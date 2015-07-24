//
//  GameLayer.h
//  SlidePuzzle
//
//  Created by 吉田　理貴 on 2015/05/17.
//
//

#ifndef __SlidePuzzle__GameLayer__
#define __SlidePuzzle__GameLayer__

#include "cocos2d.h"
#include "random"
#include "GridSprite.h"

#define GRID_NUM_X 3
#define GRID_NUM_Y 3

#define ONE_ACTION_TIME 0.5

#define WINSIZE Director::getInstance()->getWinSize() // 画面サイズ取得用マクロ

class GameLayer : public cocos2d::Layer
{
protected:
    // Zオーダー
    enum ZOrder
    {
        Background,
        EmptyGrid,
        FullGrid,
        MovingGrid,
        ButtonBackground,
        ButtonCharacter,
        Balloon,
    };
    
    // Game状態
    enum GameState
    {
        Initial, // 初期状態
        AgitatingGrid, // グリッドをかき回す
        Started, // ゲームスタート
        Ended, // ゲームエンド
    };
    
    enum MovedDirection
    {
        Up = 0, // 上方向に移動
        Down = 1, // 下方向に移動
        Right = 2, // 右方向に移動
        Left = 3, // 左方向に移動
        Current = 4, // 移動していない
    };
    
    std::default_random_engine _engine; // 乱数生成エンジン
    
    GridSprite* _movingGrid; // 動かしているグリッド
    GridSprite* _emptyGrid; // 空のグリッド
    
    void initBackground(); // 背景の初期化
    void initGrids(); // グリッドの初期表示
    void initButtonCharacter(); // ボタンキャラの初期表示
    void initBalloon(); // 吹き出しの初期表示
    GridSprite* newGrid(GridSprite::GridType gridType, GridSprite::PositionIndex positionIndex); // 新規グリッド作成
    GridSprite* getTouchGrid(cocos2d::Point touchPos, GridSprite::PositionIndex withoutPosIndex = GridSprite::PositionIndex()); // タッチしたグリッドを取得
    bool isButtonCharacterTouched(cocos2d::Point touchPos); // ボタンキャラクターをタッチしたかどうか
    void movedGrid(); // タップ操作によるグリッドの移動完了時処理
    void replaceGridPosition(GridSprite* movingGrid, GridSprite* movedGrid); // グリッドの位置を交換する
    bool positionReplaceable(GridSprite* movingGrid, GridSprite* movedGrid); // 交換可能か
    std::string getWording(); // 吹き出し用文言の取得
    void shuffleGrid(); // グリッドのシャッフル
    bool isGameFinished(); // ゲームが終了したかの判定
    
    CC_SYNTHESIZE(GameState, _gameState, GameState); // ゲームの状態
    CC_SYNTHESIZE(MovedDirection, _movedDirection, MovedDirection); // ランダムが移動した方向
    CC_SYNTHESIZE_READONLY(int, _buttonCharacterTag, ButtonCharacterTag); // ボタンキャラクターのタグ
    
    cocos2d::Label* _wording; // 吹き出しの文言
    
public:
    GameLayer(); // コンストラクタ
    virtual bool init(); // 初期化
    CREATE_FUNC(GameLayer); // create関数生成
    static cocos2d::Scene* createScene(); // シーン生成
    
    virtual void update(float delta) ;
    
    // シングルタップイベント
    virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* unused_event);
    virtual void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* unused_event);
    virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* unused_event);
    virtual void onTouchCancelled(cocos2d::Touch* touch, cocos2d::Event* unused_event);
};

#endif /* defined(__SlidePuzzle__GameLayer__) */
