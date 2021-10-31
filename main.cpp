#include <iostream>
#include <ctime>
#include <cmath>
#include <cwchar>
#include <string>
#include "SDL2/SDL.h"
#include <vector>
#include <algorithm>

#define WINDOW_W     650
#define WINDOW_H     550

//  TODO: Enable adding text to cards

// Component of a card which tells if its facing up or down. (down ==> reveal card color)
enum CardState { FACE_UP, FACE_DOWN };
// A class to represent cards
class Card {
public:
    Card() {}
    ~Card() {}

    void OnClick() {
        // Flip card state. Simple!
        if(this->state == FACE_DOWN)
            setCardState(FACE_UP);
        else if (this->state == FACE_UP)
            setCardState(FACE_UP);
    }

    void Render(SDL_Renderer* ren) {
        if(state == FACE_DOWN)
            SDL_SetRenderDrawColor(ren, def_color.r, def_color.g, def_color.b, def_color.a);
        else if(state == FACE_UP)
            SDL_SetRenderDrawColor(ren, color.r, color.g, color.b, color.a);

        if(&rect)
            SDL_RenderFillRect(ren, &this->rect);
        
    }

    void setRect(SDL_Rect _r) { this->rect = _r; }
    void setCardState(CardState _cardState) { 
        if(!stateLockOn)
            this->state = _cardState; 
    }
    void lockState() { this->stateLockOn = true; }
    void unLockState() { this->stateLockOn = false; }
    bool isLocked() { return this->stateLockOn; }
    CardState* getCardState() { return &this->state; }
    void setColor(SDL_Color _color) { this->color = _color; }
    SDL_Color getColor() { return this->color; }
    void getPosition(int* x, int* y, int* w, int* h) {
        if(x)
            *x = this->rect.x;
        if(y)
            *y = this->rect.y;
        if(w)
            *w = this->rect.w;
        if(h)
            *h = this->rect.h;
    }

private:
    CardState state = FACE_DOWN;
    SDL_Color def_color = { 255, 0, 0, 255 };
    SDL_Color color = { 100, 100, 100, 255 };
    SDL_Rect rect;
    bool stateLockOn = false;
};

#pragma region 
//#region [rgba(100, 100, 255, 0.03)]

// Returns a matrix populatex with random integers
std::vector<std::vector<int>> generateColorPattern(int _size){
    std::vector<std::vector<int>> matrix;
    int size = _size;
    if(!(_size % 2 == 0))
        size+=1;
    for(int i = 0; i < size; i++)  {
        std::vector<int> tmp;
        matrix.push_back(tmp);
    }
    std::vector<int> list_of_numbers;
    for(int i = 0; i < size*size; i++) {
        int random_number = rand() % (size*size/2);
        int number_count = std::count(list_of_numbers.begin(), list_of_numbers.end(), random_number);

        if(number_count == 1) {
            matrix[i/size].push_back(random_number);
            list_of_numbers.push_back(random_number);
        } 
        else if (number_count < 1) {
            if((int)list_of_numbers.size() < size*size) { 
                matrix[i/size].push_back(random_number);
                list_of_numbers.push_back(random_number);
            }
            else {
                i--;
            }
       }
       else {
           i--;
       }
    }

    for(size_t i = 0; i < list_of_numbers.size(); i++) {
        if(std::count(list_of_numbers.begin(), list_of_numbers.end(), list_of_numbers[i]) > 2)
            std::cout << "\npattern generation error.\n";
        if(std::count(list_of_numbers.begin(), list_of_numbers.end(), list_of_numbers[i]) < 2)
            std::cout << "\npattern generation error.\n";
    }

   return matrix;
}

Card* getCard(std::vector<Card*> _clist, int mouse_x, int mouse_y) {
    int x = 0, y = 0;
    int w = 0, h = 0;
    for(auto&& c : _clist) {
       c->getPosition(&x, &y, &w, &h);
       if(mouse_x > x && mouse_x < x+w && mouse_y > y && mouse_y < y+h) {
           return c;
       }
    }
    return NULL;
}

SDL_Color hex2sdl(std::string input) {
    if (input[0] == '#')
        input.erase(0, 1);

    unsigned long value = std::stoul(input, nullptr, 16);

    SDL_Color color;
    color.a = (value >> 24) & 0xff;
    color.r = (value >> 16) & 0xff;
    color.g = (value >> 8) & 0xff;
    color.b = (value >> 0) & 0xff;
    return color;
}

void hex_string(char str[], int length)
{
    char hex_characters[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
  
    int i;
    for(i=0;i<length;i++)
    {
        str[i]=hex_characters[rand()%16];
    }
    str[length]=0;
}

//#endregion
#pragma endregion

#pragma region
// #region [rgba(100, 255, 100, 0.03)]
int main(int agrc, char* argv[]) {
    // SDL INITIALIZATION VARIABLES
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window;
    window = SDL_CreateWindow("Memory Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_W, WINDOW_H, 0);
    SDL_assert(window);

    SDL_Renderer *renderer;
    renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_assert(renderer);

//#endregion
    // OTHER VARIABLE INITIALIZATION
//#region [rgba(100, 255, 100, 0.06)]
    bool isRunning = true;

    int number_rw = 4;

    int window_margin = 15;

    int card_w = (WINDOW_W - (window_margin*2) - 150) / number_rw - 5;
    int card_h = (WINDOW_H - (window_margin*2)) / number_rw - 5;

    std::vector<std::vector<int>> mat = generateColorPattern(number_rw);
    std::vector<Card*> cards_list;

    SDL_Palette color_pallete;
    color_pallete.ncolors = (number_rw*number_rw) / 2;
    SDL_Color *colors_arr = new SDL_Color[color_pallete.ncolors];

    char* str = (char*)malloc(8*sizeof(char));
    
    // Assign colors to color_palette
    srand(time(0));
    for(int i = 0; i < color_pallete.ncolors; i++) {
        // Assigning a certain color
        hex_string(str, 6);
        SDL_Color card_color = hex2sdl(str);
        colors_arr[i] = card_color;
    }
    color_pallete.colors = colors_arr;
    free(str);

    std::cout << "color_palette.ncolors: " << color_pallete.ncolors << std::endl;
    std::cout << "------------- Colors -------------\n";
    for(int i = 0; i < color_pallete.ncolors; i++) {
        std::cout << i << ". " << (int)color_pallete.colors[i].r << ", " << (int)color_pallete.colors[i].g << ", " << (int)color_pallete.colors[i].b << std::endl;
    }

    // Creating Cards
    // Assigning colors to cards
    int index = 0;
    while (index < (number_rw*number_rw)) {
        Card* c = new Card();
        SDL_Rect tmp_r = { ((index % number_rw)*card_w)+(index % number_rw * 5) + window_margin, (((index / number_rw))*card_h)+(index / number_rw * 5) + window_margin, card_w, card_h };
        c->setRect(tmp_r);
        c->setColor(color_pallete.colors[mat[index % number_rw][index / number_rw]]);
        c->setCardState(FACE_DOWN);

        cards_list.push_back(c);
        index++;
    }

    unsigned long t1 = 0, t2 = 0;;
    float elapsed = 0.0f;

    // GAME-SPECIFIC VARIABLES
    int selected_cards = 0;
    std::vector<Card*> unfolded_cards;

    std::cout << "------------- Pattern -------------" << std::endl;
    for(int i = 0; i < number_rw*number_rw; i++) {
        std::cout << mat[i % number_rw][i / number_rw] << " ";
        if((i+1) % number_rw == 0)
            std::cout << std::endl;
    }

//#endregion
//////////////// MAIN LOOP ////////////////
// #region [rgba(100, 255, 100, 0.03)]
    while(isRunning) {
        t1 = SDL_GetTicks();
    // HANDLE EVENT
        SDL_Event e;
        while(SDL_PollEvent(&e)) {
            switch (e.type)
            {
            case SDL_QUIT:
                isRunning = false;
                break;
            case SDL_MOUSEBUTTONDOWN:
                switch(e.button.button)
                {
                    // Most of the Game logic is right here.
                    case SDL_BUTTON_LEFT: {
                        int x, y;
                        SDL_GetMouseState(&x, &y);
                        Card* selected_card = getCard(cards_list, x, y);

                        // TODO: Might want to implement functions here
                        if(selected_card) {
                            if(*selected_card->getCardState() == FACE_DOWN && selected_cards < 2) {
                                getCard(cards_list, x, y)->OnClick();
                                selected_cards++;
                            } else if(*selected_card->getCardState() == FACE_UP && !selected_card->isLocked()) {
                                // selected_card->OnClick();
                                selected_card->setCardState(FACE_DOWN);
                                selected_cards--;
                                std::cout << "selecetd card is facing up.\n";
                            } else if(selected_cards >= 2) {
                                selected_cards = 0;
                                for(int i = 0; i < (int)cards_list.size(); i++) {
                                    cards_list[i]->setCardState(FACE_DOWN);
                                }
                            }
                        } else {
                                selected_cards = 0;
                                for(int i = 0; i < (int)cards_list.size(); i++) {
                                    cards_list[i]->setCardState(FACE_DOWN);
                                }
                        }

                        if(selected_cards == 2) {
                            Card* card1 = NULL;
                            Card* card2 = NULL;

                            for(auto&& c : cards_list) {
                                if(*c->getCardState() == FACE_UP && !c->isLocked()) {
                                    if(!card1) {
                                        card1 = c;
                                    }
                                    else if(!card2) {
                                        card2 = c;
                                        break;
                                    }
                                }
                            }

                            if(card1 && card2 && card1->getColor().r == card2->getColor().r &&
                            card1->getColor().g == card2->getColor().g && 
                            card1->getColor().b == card2->getColor().b) {
                                unfolded_cards.push_back(card1);
                                unfolded_cards.push_back(card2);
                                card1->lockState();
                                card2->lockState();
                                for(auto& c : unfolded_cards) {
                                    int x, y;
                                    c->getPosition(&x, &y, NULL, NULL);
                                    std::cout << "card: [" << (x/90)+1 << ", " << (y/100)+1 << "] ";
                                }

                                std::cout << std::endl;
                                card1 = NULL;
                                card2 = NULL;
                                selected_cards = 0;
                            }

                            selected_card = NULL;
                        }
                    }
                    default:
                        break;
                }
                break;
            case SDL_KEYDOWN:
                for(auto&& c : cards_list) {
                    c->unLockState();
                    c->setCardState(FACE_DOWN);
                }
                unfolded_cards.clear();
                break;
            default:
                break;
            }
        }
    // UPDATE
       //
    // RENDER
        SDL_SetRenderDrawColor(renderer, 242, 242, 242, 255);
        SDL_RenderClear(renderer);          
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        for(auto&& c : cards_list) {
            c->Render(renderer);
        }
        SDL_RenderPresent(renderer);


    // FRAME LIMITER THING... IDK
        t2 = SDL_GetTicks();
        elapsed = (t2 - t1) / (float)SDL_GetTicks();
        SDL_Delay(std::floor(16.666f - elapsed));
    }

/////////// -- MAIN LOOP END -- /////////// 

    // CLEANUP 
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();

    // Free cards from memory
    for(auto&& o : cards_list) {
        delete o;
    }
    delete[] colors_arr;

    std::cout << "Program ended succesfully.\n";

    return 0;
}
//#endregion
#pragma endregion