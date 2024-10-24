#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

const string CARS_FILE = "cars.csv";
const string PARKING_FILE = "slots.csv";
const string PRICE_FILE = "prices.csv";

struct Car {
    string name;
    int size;
    int salam;
};

struct ParkingLot {
    int id;
    int size;
    string type;
    bool occupied = false;
    int cost = 0;
};

struct LotPrice {
    int size;
    int staticPrice;
    int pricePerDay;
};

vector<string> splitStringBy(const string& sentence, char delimiter) {
    vector<string> words;
    istringstream stream(sentence);
    string word;

    while(getline(stream, word, delimiter)) {
        words.push_back(word);
    }

    return words;
}
void handleArg(int argc, char* argv[],string& CARS_FILE,string& PARKING_FILE,string& PRICE_FILE) {
    CARS_FILE = argv[1];
    PARKING_FILE = argv[2];
    PRICE_FILE = argv[3];
}

void handleCarsCsv(string fileAddress, vector<Car>& cars) {
    ifstream file(fileAddress);
    string line;
    getline(file, line);

    while(getline(file, line)) {

        Car car;
        string temp;
        stringstream lineStream(line);
        getline(lineStream, car.name, ',');
        getline(lineStream, temp, ',');
        car.size = stoi(temp);
        cars.push_back(car);
    }
    file.close();
}

void handleSlotsCsv(string fileAddress, vector<ParkingLot>& slots) {
    ifstream file(fileAddress);
    string line;
    getline(file, line);

    while(getline(file, line)) {

        ParkingLot slot;
        string temp;
        stringstream lineStream(line);
        getline(lineStream, temp, ',');
        slot.id = stoi(temp);
        getline(lineStream, temp, ',');
        slot.size = stoi(temp);
        getline(lineStream, slot.type, ',');
        slots.push_back(slot);
    }
    file.close();
}

void handlePricesCsv(string fileAddress, vector<LotPrice>& prices) {
    ifstream file(fileAddress);
    string line;
    getline(file, line);

    while(getline(file, line)) {

        LotPrice price;
        string temp;
        stringstream lineStream(line);
        getline(lineStream, temp, ',');
        price.size = stoi(temp);
        getline(lineStream, temp, ',');
        price.staticPrice = stoi(temp);
        getline(lineStream, temp, ',');
        price.pricePerDay = stoi(temp);
        prices.push_back(price);
    }
    file.close();
}

bool compareById(ParkingLot slot1, ParkingLot slot2) {
    return slot1.id < slot2.id;
}

int getCarSize(vector<Car> cars, string carName) {
    auto it = find_if(cars.begin(), cars.end(), [carName](Car car){
        return car.name == carName;
    });

    return it->size;
}

vector<ParkingLot> getAvailableSpots(vector<ParkingLot>& spots, int carSize) {
    vector<ParkingLot> availableSpots;

    for (auto spot : spots) {
        if (carSize == spot.size and !(spot.occupied))
            availableSpots.push_back(spot);
    }

    sort(availableSpots.begin(), availableSpots.end(), compareById);

    return availableSpots;
}

LotPrice getSizePrice(vector<LotPrice> prices, int size) {
    auto it = find_if(prices.begin(), prices.end(), [size](LotPrice price){
        return price.size == size;
    });

    return *it;
}

int calculateStaticPrice(ParkingLot& spot, LotPrice spotPrice) {
    if (spot.type == "normal"){
        return spotPrice.staticPrice;
    }
    else if (spot.type == "covered") {
        return spotPrice.staticPrice + 50;
    }
    else if (spot.type == "CCTV") {
        return spotPrice.staticPrice + 80;
    }

    return 0;
}

int calculatePricePerDay(ParkingLot& spot, LotPrice spotPrice) {
    if (spot.type == "normal"){
        return spotPrice.pricePerDay;
    }
    else if (spot.type == "covered") {
        return spotPrice.pricePerDay + 30;
    }
    else if (spot.type == "CCTV") {
        return spotPrice.pricePerDay + 60;
    }
    else {
        cout << "unknown spot type!" << endl;
    }

    return 0;
}

void displayAvailableSpots(vector<ParkingLot> availableSpots, vector<LotPrice> prices, int carSize) {
    LotPrice sizePrice = getSizePrice(prices, carSize);

    for (auto spot : availableSpots) {
        int staticPrice = calculateStaticPrice(spot, sizePrice);
        int pricePerDay = calculatePricePerDay(spot, sizePrice);
        cout << spot.id << ": " << spot.type << ' ' << staticPrice << ' ' <<  pricePerDay << endl;
    }
}

void requestSpot(vector<string> input, vector<Car> cars, vector<ParkingLot>& slots, vector<LotPrice> prices) {
    vector<ParkingLot> availableSpots;
    string carName = input[1];
    int carSize = getCarSize(cars, carName);

    availableSpots = getAvailableSpots(slots, carSize);

    displayAvailableSpots(availableSpots, prices, carSize);
}

ParkingLot& findParkingLotById(vector<ParkingLot>& spots, int id) {
    auto it = find_if(spots.begin(), spots.end(), [id](ParkingLot spot){
        return spot.id == id;
    });

    return *it;
}

void occupySpot(ParkingLot& spot, LotPrice spotPrice) {
    spot.occupied = true;
    spot.cost = calculateStaticPrice(spot, spotPrice);
}

void assignSpot(vector<string> input, vector<ParkingLot>& slots, vector<LotPrice> prices) {
    int id = stoi(input[1]);

    ParkingLot& spot = findParkingLotById(slots, id);
    LotPrice sizePrice = getSizePrice(prices, spot.size);
    occupySpot(spot, sizePrice);

    cout << "Spot " << id << " is occupied now." << endl;
}

void freeSpot(ParkingLot& spot) {
    spot.occupied = false;
    spot.cost = 0;
}

void checkout(vector<string> input, vector<ParkingLot>& slots) {
    int id = stoi(input[1]);
    ParkingLot& spot = findParkingLotById(slots, id);
    cout << "Spot " << id << " is free now." << endl;
    cout << "Total cost: " << spot.cost << endl;
    freeSpot(spot);
}

void passTime(vector<string> input, vector<ParkingLot>& spots, vector<LotPrice> prices) {
    int days = stoi(input[1]);
    for (auto& spot : spots) {
        if (spot.occupied) {
            LotPrice normalPrice = getSizePrice(prices, spot.size);
            spot.cost += days * calculatePricePerDay(spot, normalPrice);
        }
    }
}

void handleInput(vector<Car>& cars, vector<ParkingLot>& slots, vector<LotPrice>& prices) {
    string input;
    while (getline(cin, input)) {
        vector<string> argument = splitStringBy(input, ' ');
        string command = argument[0];
        if (command == "request_spot")
            requestSpot(argument, cars, slots, prices);
        if (command == "assign_spot")
            assignSpot(argument, slots, prices);
        if (command == "checkout")
            checkout(argument, slots);
        if (command == "pass_time")
            passTime(argument, slots, prices);
    }
}

int main(int argc, char* argv[]) {
    vector<Car> cars;
    vector<ParkingLot> slots;
    vector<LotPrice> prices;
    string CARS_FILE;
    string PARKING_FILE;
    string PRICE_FILE;
    
    handleArg(argc, argv, CARS_FILE, PARKING_FILE, PRICE_FILE);
    
    handleCarsCsv(CARS_FILE, cars);
    handleSlotsCsv(PARKING_FILE, slots);
    handlePricesCsv(PRICE_FILE, prices);
    handleInput(cars, slots, prices);

    return 1;
}
