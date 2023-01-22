#ifndef GAME_H
#define GAME_H
class Game{
public:
  Game();
  ~Game();
  int run() ;
private:
  Game( const Game& );
  Game& operator= ( const Game& );
  class GameImpl* pimpl ;
};
#endif
