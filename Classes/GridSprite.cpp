//
//  GridSprite.cpp
//  SlidePuzzle
//
//  Created by 吉田　理貴 on 2015/05/17.
//
//

#include "GridSprite.h"

USING_NS_CC;

// コンストラクタ
GridSprite::GridSprite()
: _checkedX(false)
, _checkedY(false)
, _isHomePosition(true)
, _positionIndex(0, 0)
{
}

// インスタンス生成
GridSprite* GridSprite::create(PositionIndex positionIndex, GridType gridType, int gridNum, bool visible)
{
    GridSprite* pRet = new GridSprite();
    if (pRet && pRet->init(positionIndex, gridType, gridNum, visible)) {
        pRet->autorelease();
    } else {
        delete pRet;
        pRet = nullptr;
    }
    
    return pRet;
}

// 初期化
bool GridSprite::init(PositionIndex positionIndex, GridType gridType, int gridNum, bool visible)
{
    if (!Sprite::initWithFile(getGridImageFilePath(gridType))) {
        return false;
    }
    
    _gridType = gridType;
    _gridNum = gridNum;
    _homePosition = positionIndex;
    showNumber();
    
    setVisible(visible);
    return true;
}

// パラメータのリセット
void GridSprite::resetParams()
{
    _checkedX = false;
    _checkedY = false;
}

// 初期位置へ移動
void GridSprite::resetPosition()
{
    // 位置を変更する
    setPosition(getPositionForPositionIndex(_positionIndex));
}

// 画像を変更
void GridSprite::changeTexture()
{
    setTexture(getGridImageFilePath(_gridType));
}

// 位置インデックスを返す
GridSprite::PositionIndex GridSprite::getPositionIndex()
{
    // 位置インデックスを返す
    return _positionIndex;
}

// 位置インデックスとタグを変更する
void GridSprite::setPositionIndex(PositionIndex positionIndex)
{
    // 位置インデックスを保持する
    _positionIndex = positionIndex;
    
    // タグをセットする
    setTag(getGenerateTag(_positionIndex));
}

// インデックスとタグと位置を変更する
void GridSprite::setPositionIndexAndChangePosition(PositionIndex positionIndex)
{
    // インデックスとタグを変更する
    setPositionIndex(positionIndex);
    
    // 位置を変更する
    resetPosition();
}

// グリッド画像の取得
std::string GridSprite::getGridImageFilePath(GridType gridType)
{
    _isHomePosition = isHomePosition();
    switch (gridType) {
        case GridType::Full:
            if (_isHomePosition) {
                return "homeGrid.png";
            } else {
                return "fullGrid.png";
            }
        case GridType::Empty:
            return "emptyGrid.png";
    }
}

// 位置インデックスからPointを取得
Point GridSprite::getPositionForPositionIndex(PositionIndex positionIndex)
{
    return Point(GRID_SIZE * (positionIndex.x - 0.5) + 1,
                 GRID_SIZE * (positionIndex.y - 0.5) + WINSIZE.height / 3);
}

// 位置インデックスからタグを取得
int GridSprite::getGenerateTag(PositionIndex positionIndex)
{
    return positionIndex.x * 10 + positionIndex.y;
}

void GridSprite::showNumber()
{
    std::string numberString = StringUtils::format("%d", _gridNum);
    Color4B textColor = Color4B::BLACK;
    
    auto number = Label::createWithSystemFont(numberString, "Arial", 96);
    number->setPosition(Point(getContentSize() / 2));
    number->setTextColor(textColor);
    addChild(number);
}

// グリッドが定位置にいるか
bool GridSprite::isHomePosition()
{
    auto positionIndex = getPositionIndex();
    auto homePosition = getHomePosition();
    return (positionIndex.x == homePosition.x && positionIndex.y == homePosition.y);
}
