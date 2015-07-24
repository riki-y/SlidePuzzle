//
//  GridSprite.h
//  SlidePuzzle
//
//  Created by 吉田　理貴 on 2015/05/17.
//
//

#ifndef __SlidePuzzle__GridSprite__
#define __SlidePuzzle__GridSprite__

#include "cocos2d.h"

#define GRID_SIZE 212 // グリッドのサイズ

#define WINSIZE Director::getInstance()->getWinSize() // 画面サイズ取得用マクロ

class GridSprite : public cocos2d::Sprite
{
public:
    // グリッドの種類
    enum class GridType
    {
        Full, // グリッドに数字が入っている
        Empty, // グリッドに数字が入っていない
    };
    
    // 位置インデックス
    struct PositionIndex {
        // コンストラクタ
        PositionIndex() {
            x = 0;
            y = 0;
        }
        
        // コンストラクタ
        PositionIndex(int _x, int _y) {
            x = _x;
            y = _y;
        }
        
        int x; // x方向のインデックス
        int y; // y方向のインデックス
    };
    
    GridSprite(); // コンストラクタ
    static GridSprite* create(PositionIndex positionIndex, GridType gridType, int gridNum, bool visible); // インスタンス生成
    virtual bool init(PositionIndex positionIndex, GridType gridType, int gridNum, bool visible); // 初期化
    
    CC_SYNTHESIZE(bool, _checkedX, CheckedX); // 横方向確認フラグ
    CC_SYNTHESIZE(bool, _checkedY, CheckedY); // 縦方向確認フラグ
    CC_SYNTHESIZE(bool, _isHomePosition, IsHomePosition); // 定位置にいるか
    CC_SYNTHESIZE_READONLY(int, _gridNum, GridNum) // グリッドの番号
    CC_SYNTHESIZE_READONLY(GridType, _gridType, GridType); // グリッドの種類
    CC_SYNTHESIZE_READONLY(PositionIndex, _homePosition, HomePosition); // 定位置
    CC_PROPERTY(PositionIndex, _positionIndex, PositionIndex); // 位置インデックス
    
    void setPositionIndexAndChangePosition(PositionIndex positionIndex); // 位置インデックスの設定および位置変更
    void resetParams(); // パラメータのリセット
    void resetPosition(); // 初期位置へ移動
    void changeTexture(); // 画像を変更
    
    std::string getGridImageFilePath(GridType gridType); // グリッド画像取得
    static cocos2d::Point getPositionForPositionIndex(PositionIndex positionIndex); // 位置インデックスからタグを取得
    static int getGenerateTag(PositionIndex positionIndex); // 位置インデックスからタグを取得
    
protected:
    void showNumber(); // グリッドにマークを表示
    bool isHomePosition(); // グリッドが定位置にいるか
};

#endif /* defined(__SlidePuzzle__GridSprite__) */
