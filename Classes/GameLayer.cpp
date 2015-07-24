//
//  GameLayer.cpp
//  SlidePuzzle
//
//  Created by 吉田　理貴 on 2015/05/17.
//
//

#include "GameLayer.h"

USING_NS_CC;

// コンストラクタ
GameLayer::GameLayer()
: _movingGrid(nullptr)
, _gameState(Initial)
, _buttonCharacterTag(WINSIZE.width/2*10+WINSIZE.height/6)
, _movedDirection(Current)
{
    std::random_device device;
    _engine = std::default_random_engine(device());
}

// シーンの生成
Scene* GameLayer::createScene()
{
    auto scene = Scene::create();
    auto layer = GameLayer::create();
    scene->addChild(layer);
    
    return scene;
}

// 初期化
bool GameLayer::init()
{
    if (!Layer::init()) {
        return false;
    }
    
    // シングルタップイベントの取得
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(_swallowsTouches);
    touchListener->onTouchBegan = CC_CALLBACK_2(GameLayer::onTouchBegan, this);
    touchListener->onTouchMoved = CC_CALLBACK_2(GameLayer::onTouchMoved, this);
    touchListener->onTouchEnded = CC_CALLBACK_2(GameLayer::onTouchEnded, this);
    touchListener->onTouchCancelled = CC_CALLBACK_2(GameLayer::onTouchCancelled, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
    
    initBackground(); // 背景の初期化
    initGrids(); // グリッドの初期表示
    initButtonCharacter(); // ボタンキャラの初期表示
    initBalloon(); // 吹き出しの初期表示
    
    return true;
}

// 背景の初期化
void GameLayer::initBackground()
{
    auto background = Sprite::create("background.png");
    background->setAnchorPoint(Point::ZERO);
    background->setPosition(Point::ZERO);
    addChild(background, ZOrder::Background);
}

// ボールの初期表示
void GameLayer::initGrids()
{
    for (int x = 1; x <= GRID_NUM_X; x++) {
        for (int y = 1; y <= GRID_NUM_Y; y++) {
            if (x == GRID_NUM_X && y == 1) {
                // 空グリッドを生成する
                _emptyGrid = newGrid(GridSprite::GridType::Empty, GridSprite::PositionIndex(x, y));
            } else {
                // フルグリッドを生成する
                newGrid(GridSprite::GridType::Full, GridSprite::PositionIndex(x, y));
            }
        }
    }
}

// ボタンキャラの初期表示
void GameLayer::initButtonCharacter()
{
    auto buttonBackground = Sprite::create("buttonBackground.png");
    buttonBackground->setPosition(WINSIZE.width/2, WINSIZE.height/6);
    addChild(buttonBackground, ZOrder::ButtonBackground);
    
    auto buttonCharacter = Sprite::create("buttonCharacter.png");
    buttonCharacter->setPosition(WINSIZE.width/2, WINSIZE.height/6);
    buttonCharacter->setTag(_buttonCharacterTag);
    addChild(buttonCharacter, ZOrder::ButtonCharacter);
    
    auto scale1 = ScaleTo::create(ONE_ACTION_TIME / 2, 0, 1);
    auto scale2 = ScaleTo::create(ONE_ACTION_TIME / 2, 1, 1);
    auto delay = CCDelayTime::create(5.000f);
    auto scale3 = ScaleTo::create(ONE_ACTION_TIME / 2, 1, 0);
    auto scale4 = ScaleTo::create(ONE_ACTION_TIME / 2, 1, 1);
    auto seq = Sequence::create(delay, scale1, scale2, delay, scale3, scale4, nullptr);
    auto repeatForever = CCRepeatForever::create(seq);
    
    buttonCharacter->runAction(repeatForever);
    
}

// 吹き出しの初期表示
void GameLayer::initBalloon()
{
    auto balloon = Sprite::create("balloon.png");
    balloon->setPosition(WINSIZE.width*3/4+10, WINSIZE.height/4);
    addChild(balloon, ZOrder::Balloon);
    
    Color4B textColor = Color4B::BLACK;
    
    _wording = Label::createWithSystemFont(getWording(), "Arial", 30);
    _wording->setPosition(balloon->getContentSize().width/2, balloon->getContentSize().height/2+10);
    _wording->setTextColor(textColor);
    balloon->addChild(_wording);
    
}

// 新規グリッドの作成
GridSprite* GameLayer::newGrid(GridSprite::GridType gridType, GridSprite::PositionIndex positionIndex)
{
    // 現在のタグを取得
    //int currentTag = GridSprite::getGenerateTag(positionIndex);
    
    int gridNumber = (GRID_NUM_X*GRID_NUM_Y) - (3 * positionIndex.y - positionIndex.x);
    auto grid = GridSprite::create(positionIndex, gridType, gridNumber, true);
    grid->setPositionIndexAndChangePosition(positionIndex);
    switch (gridType) {
        case GridSprite::GridType::Full:
            addChild(grid, ZOrder::FullGrid);
            break;
        case GridSprite::GridType::Empty:
            addChild(grid, ZOrder::EmptyGrid);
            break;
    }
    
    return grid;
}

bool GameLayer::onTouchBegan(Touch* touch, Event* unused_event)
{
    
    if (isButtonCharacterTouched(touch->getLocation())) {
        if (_gameState == GameState::Initial || _gameState == GameState::Ended) {
            _gameState = GameState::AgitatingGrid;
            _movedDirection = Current;
            scheduleUpdate();
        } else if (_gameState == GameState::AgitatingGrid) {
            _gameState = GameState::Started;
            unscheduleUpdate();
        }
        
        _wording->setString(getWording());
        return false;
    }
    
    if (_gameState == GameState::Started) {
        _movingGrid = getTouchGrid(touch->getLocation());
        
        if (_movingGrid && _movingGrid->getGridType() == GridSprite::GridType::Full) {
            reorderChild(_movingGrid, ZOrder::MovingGrid);
            return true;
        }
    }
    
    return false;
}

void GameLayer::onTouchMoved(Touch* touch, Event* unused_event)
{
    // スワイプとともにグリッドを移動する
    _movingGrid->setPosition(_movingGrid->getPosition() + touch->getDelta());
    
    auto touchGrid = getTouchGrid(touch->getLocation(), _movingGrid->getPositionIndex());
    if (touchGrid && _movingGrid != touchGrid && positionReplaceable(_movingGrid, touchGrid)) {
        
        // グリッドを変更
        replaceGridPosition(_movingGrid, touchGrid);
        
    }
}

void GameLayer::onTouchEnded(Touch* touch, Event* unused_event)
{
    // タップ操作によるボールの移動完了時の処理
    movedGrid();
    
    // ゲームの終了判定
    if (isGameFinished()) {
        _gameState = GameState::Ended;
        _wording->setString(getWording());
    }
    
}

void GameLayer::onTouchCancelled(Touch* touch, Event* unused_event)
{
    onTouchEnded(touch, unused_event);
}

void GameLayer::update(float delta) {
    shuffleGrid();
}

// タップした位置のチェック
GridSprite* GameLayer::getTouchGrid(Point touchPos, GridSprite::PositionIndex withoutPosIndex)
{
    for (int x = 1; x <= GRID_NUM_X; x++) {
        for (int y = 1; y <= GRID_NUM_Y; y++) {
            if (x == withoutPosIndex.x && y == withoutPosIndex.y) {
                // 指定位置のボールの場合は、以下の処理を行わない
                continue;
            }
            
            // タップ位置にあるボールかどうかを判断する
            int tag = GridSprite::getGenerateTag(GridSprite::PositionIndex(x, y));
            auto grid = (GridSprite*)(getChildByTag(tag));
            if (grid) {
                // 2点間の距離を求める
                float distance = grid->getPosition().getDistance(touchPos);
                
                if (distance <= GRID_SIZE / 2) {
                    return grid;
                }
            }
        }
    }
    
    return nullptr;
}

// ボタンキャラクターをタッチしたかどうか
bool GameLayer::isButtonCharacterTouched(Point touchPos) {
    //TODO
    auto buttonCharacter = (Sprite*)(getChildByTag(_buttonCharacterTag));
    if (buttonCharacter) {
        // 2点間の距離を求める
        float distance = buttonCharacter->getPosition().getDistance(touchPos);
        if (distance <= 100) {
            return true;
        }
    }
    
    return false;
}

// タップ操作によるボールの移動完了時処理
void GameLayer::movedGrid()
{
    // 移動しているグリッドを本来の位置に戻す
    _movingGrid->resetPosition();
    _movingGrid->changeTexture();
    reorderChild(_movingGrid, ZOrder::FullGrid);
    
    _movingGrid = nullptr;
}

// グリッドの位置を交換する
void GameLayer::replaceGridPosition(GridSprite* movingGrid, GridSprite* movedGrid)
{
    // 別のグリッドの位置インデックスを取得
    auto movedGridPositionIndex = movedGrid->getPositionIndex();
    
    // 別のグリッドを移動しているグリッドの元の位置へ移動する
    movedGrid->setPositionIndexAndChangePosition(movingGrid->getPositionIndex());
    
    // 移動しているグリッドの情報を変更
    movingGrid->setPositionIndex(movedGridPositionIndex);
}

// 交換可能か
bool GameLayer::positionReplaceable(GridSprite* movingGrid, GridSprite* movedGrid)
{
    if (movedGrid->getGridType() == GridSprite::GridType::Empty) {
        auto movingPosition = movingGrid->getPositionIndex();
        auto movedPosition = movedGrid->getPositionIndex();
        
        if (((movingPosition.x == movedPosition.x - 1 || movingPosition.x == movedPosition.x + 1)
             && movingPosition.y == movedPosition.y) ||
            ((movingPosition.y == movedPosition.y - 1 || movingPosition.y == movedPosition.y + 1)
             && movingPosition.x == movedPosition.x)) {
                return true;
            }
    }
    
    return false;
}

// 吹き出し用文言の取得
std::string GameLayer::getWording()
{
    switch (_gameState) {
        case Initial:
            return "TOUCH ME!";
        case AgitatingGrid:
            return "START?";
        case Started:
            return "LET'S TRY!";
        case Ended:
            return "PERFECT!";
        default:
            return "...";
    }
}

// グリッドのシャッフル
void GameLayer::shuffleGrid()
{
    
    double up = 20;
    double down = 20;
    double right = 20;
    double left = 20;
    
    int emptyPositionX = _emptyGrid->getPositionIndex().x;
    int emptyPositionY = _emptyGrid->getPositionIndex().y;
    
    // 移動できない方向は出ないようにする
    if (emptyPositionX == 1) {
        left = 0;
    }
    
    if (emptyPositionX == GRID_NUM_X) {
        right = 0;
    }
    
    if (emptyPositionY == 1) {
        down = 0;
    }
    
    if (emptyPositionY == GRID_NUM_Y) {
        up = 0;
    }
    
    // 一個前に来た方向は出ないようにする
    switch (_movedDirection) {
        case MovedDirection::Up:
            down = 0;
            break;
        case MovedDirection::Down:
            up = 0;
            break;
        case MovedDirection::Right:
            left = 0;
            break;
        case MovedDirection::Left:
            right = 0;
            break;
        default:
            break;
    }
    
    // 乱数の分布
    std::discrete_distribution<int> _distForBall = std::discrete_distribution<int>{up, down, right, left};
    int movingDirection = _distForBall(_engine);

    int tag;
    GridSprite::GridSprite* grid;
    
    if (_movedDirection != movingDirection) {
        _movedDirection = (MovedDirection)movingDirection;
        switch (_movedDirection) {
            case MovedDirection::Up:
                tag = GridSprite::getGenerateTag(GridSprite::PositionIndex(emptyPositionX, emptyPositionY + 1));
                grid = (GridSprite*)(getChildByTag(tag));
                
                break;
            case MovedDirection::Down:
                tag = GridSprite::getGenerateTag(GridSprite::PositionIndex(emptyPositionX, emptyPositionY - 1));
                grid = (GridSprite*)(getChildByTag(tag));
                
                break;
            case MovedDirection::Right:
                tag = GridSprite::getGenerateTag(GridSprite::PositionIndex(emptyPositionX + 1, emptyPositionY));
                grid = (GridSprite*)(getChildByTag(tag));
                
                break;
            case MovedDirection::Left:
                tag = GridSprite::getGenerateTag(GridSprite::PositionIndex(emptyPositionX - 1, emptyPositionY));
                grid = (GridSprite*)(getChildByTag(tag));
                
                break;
            default:
                break;
        }
        
        // グリッドを変更
        replaceGridPosition(grid, _emptyGrid);
        
        grid->resetPosition();
        grid->changeTexture();
    }
}

// ゲームが終了したかの判定
bool GameLayer::isGameFinished()
{
    for (int x = 1; x <= GRID_NUM_X; x++) {
        for (int y = 1; y <= GRID_NUM_Y; y++) {
            int tag = GridSprite::getGenerateTag(GridSprite::PositionIndex(x, y));
            auto grid = (GridSprite*)(getChildByTag(tag));
            
            if (!grid->getIsHomePosition()) {
                return false;
            }
        }
    }
    
    return true;
}
