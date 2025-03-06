/*
for this stock trading scenario we have to create an addOrder function and a matchOrder function given a few constraints. These contrains 
include no imports or external libararies or defined data strucutres like maps or dictionaries. We also have to make matchOrder a o(n) 
time complexity function. The addOrder fucntion must have given parameters of order type(buy or sell), stock ticker, quantity, and price. 
We also are told to use a lock-free data strucutre and try to handle race conditions where multiple threads modify stock order book to 
best simulate real world trading with multiple stock brockers.

To solve this we decided to use a linkedlist to organize the orders and from there created the addOrder and matchOrder functions. To keep
the matchOrder at o(n) we used 2 pointers to search through the linkedlist. 

*/

//create the order class to store orders as objects 
class order{
    public:
        char orderType; // b = buy, s = sell. stored as single char for memory efficieny
        char ticker[6]; // stock ticker can be up to 5 characters depending on exhange and we include a spot for null termination
        double quantity; // how many shares, double because some stocks can be sold or purchased as fractional shares
        double price; //price of stock

        // we will use a singly linked list to process orders 
        order* next;
        order* prev;

        //intializer for objexts
        order(char orderType, const char* ticker, double quantity, double price) 
            : orderType(orderType), quantity(quantity), price(price), next(nullptr), prev(nullptr){
                //to ensure that garabage value do not mess up ticker comparisons later
                int i = 0;
                while(i < 5 && ticker[i] != '\0'){ // copies up to 5 char as long as not null
                    this->ticker[i] = ticker[i];
                    i++;
                }
                this->ticker[i] = '\0'; //ends the ticker with a null termination
            }
};

class orderBook{
    private:
        order* head;
        order* tail;
    
    public:
        orderBook() : head(nullptr), tail(nullptr) {}

        // add order fucntion adds the order as an object into a linkedlist 
        void addOrder(char orderType, const char* ticker, double quantity, double price){
            //creates the order as node for linkedList with necessary info
            order* newOrder = new order(orderType, ticker, quantity, price);
            //if list is empty
            if(!head){
                head = tail = newOrder;
            //add order to end of list and adjust tail pointer
            } else {
                tail->next = newOrder;
                newOrder->prev = tail;
                tail = newOrder;
            }
        }

        //match order function looks to match buy and sell orders
        void matchOrder(){
            //to keep runtime at o(n) we are going to use 2 ptr to search through the list, start both pointers at head of linked list
            order* buy = head; 
            order* sell = head;

            while(buy != nullptr){  //start at front of linkedlist and go until we reach end (nullptr)
                if (buy->orderType == 'b'){  //once we see a buy type we start to check for matches
                    sell = head; // make sure to start from beginning of the linkedlist to ensure all orders are being checked
                    while(sell != nullptr){ //keep checking for sell conditons thru entire list
                        //check to see if we have  match
                        if(sell->orderType == 's' && !strncmp(buy->ticker, sell->ticker) && buy->price >= sell->price){
                            //case 1: buy and sell quantities are the same so both can be removed
                            if(buy->quantity == sell->quantity){
                                removeOrder(buy);
                                removeOrder(sell);
                            //case 2 we have more buy then sell available so we buy as much as we can from that sell order
                            } else if(buy->quantity > sell -> quantity){
                                buy->quantity -= sell->quantity;
                                removeOrder(sell);
                            //case 3 we have more sell availble
                            }else{
                                sell->quantity -= buy->quantity;
                                removeOrder(buy);
                            }
                            break;
                        }
                        sell = sell->next; // go check next node in linkedlist
                    }
                
                }
                buy = buy->next; // go check next node in linkedlist
            }

        }

        // helper function to compare tickers
        int strncmp(const char* ticker1, const char* ticker2){
            int i = 0;
            while( i < 6){
                if(ticker1[i] != ticker2[i]){
                    return 1;
                } 
                if(ticker1[i] == '\0' || ticker2[i] == '\0'){
                    break;
                }
                i++;
            }
            return 0;
        }

        //helper function to remove orders from the linkelist whilst mainting the structre properly
        void removeOrder(order* order){
            if(!order){
                return;
            }
            if(order->prev){
                order->prev->next=order->next;
            }else{
                head = order->next;
            }
            if(order->next){
                order->next->prev = order->prev;
            }else{
                tail = order->prev;
            }
            delete order;
        }
};

void stockTradeSim(orderBook& orderBook){
    //randomly chosen stocks based on tickers
    const char*tickers[] = {"ONY", "NVDA", "IBM", "LEGO", "APPL"}; 
    //randomizing the order values based on loop count
    for(int i = 0; i < 1024; i++){
        char orderType = (i % 2 == 0) ? 'b' : 's';
        const char* ticker = tickers[i % 5];
        double quantity = (i % 10) + 2;
        double price = (10 + (i % 100)) * 1.3;
        //add generated order to orderbook
        orderBook.addOrder(orderType, ticker, quantity, price);
    }
}


//run sim using generated order book and functions 
int main(){
    orderBook orderBook;
    stockTradeSim(orderBook);
    orderBook.matchOrder();
    return 0;
}
