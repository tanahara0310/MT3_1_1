#include "MyMath.h"
#include <Novice.h>
#include <cstdint>

const char kWindowTitle[] = "LC1A_16_タナハラ_コア_タイトル";

//==============================
// 関数定義
//==============================

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{

    // ライブラリの初期化
    Novice::Initialize(kWindowTitle, 1280, 720);

    // キー入力結果を受け取る箱
    char keys[256] = { 0 };
    char preKeys[256] = { 0 };

    //==============================
    // 変数初期化
    //==============================

    // クロス積確認用
    /*Vector3 v1 = { 1.2f, -3.9f, 2.5f };
    Vector3 v2 = { 2.8f, 0.4f, -1.3f };

    Vector3 cross = Cross(v1, v2);*/

    Vector3 rotate = { 0.0f, 0.0f, 0.0f };
    Vector3 translate = { 0.0f, 0.0f, 5.0f };

    Vector3 cameraPos = { 0.0f, 0.0f, -5.0f };

    // ウィンドウの×ボタンが押されるまでループ
    while (Novice::ProcessMessage() == 0) {
        // フレームの開始
        Novice::BeginFrame();

        // キー入力を受け取る
        memcpy(preKeys, keys, 256);
        Novice::GetHitKeyStateAll(keys);

        ///
        /// ↓更新処理ここから
        ///

        // 前後に三角形を動かす
        if (keys[DIK_W]) {
            translate.z += 0.05f;
        }

        if (keys[DIK_S]) {
            translate.z -= 0.05f;
        }

        // 左右に三角形を動かす
        if (keys[DIK_A]) {
            translate.x -= 0.05f;
        }

        if (keys[DIK_D]) {
            translate.x += 0.05f;
        }

        // y軸回転
        rotate.y += 0.05f;

        // 背面カリング時rotate.y = 0でも表示されるかの確認用
        //rotate.y = 0.0f;

        Matrix4x4 worldMatrix = makeAffineMatrix({ 1.0, 1.0f, 1.0f }, rotate, translate);
        Matrix4x4 cameraMatrix = makeAffineMatrix({ 1.0, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, cameraPos);
        Matrix4x4 viewMatrix = Inverse(cameraMatrix);
        Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(1280) / float(720), 0.1f, 100.0f);
        Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
        Matrix4x4 viewPortMatrix = MakeViewportMatrix(0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 1.0f);

        Vector3 kLocalVertices[3] = {
            { 0.0f, 1.0f, 0.0f },
            { 1.0f, -1.0f, 0.0f },
            { -1.0f, -1.0f, 0.0f }
        };

        Vector3 screenVertices[3];

        for (uint32_t i = 0; i < 3; ++i) {
            Vector3 ndcVertex = TransformCoord(kLocalVertices[i], worldViewProjectionMatrix);
            screenVertices[i] = TransformCoord(ndcVertex, viewPortMatrix);
        }
        ///
        /// ↑更新処理ここまで
        ///

        ///
        /// ↓描画処理ここから
        ///

        // クロス積確認用
        /* VectorScreenPrintf(0, 0, cross);*/

        // 背面カリング
        Vector3 vec1 = { screenVertices[1].x - screenVertices[0].x,
            screenVertices[1].y - screenVertices[0].y,
            0.0f };

        Vector3 vec2 = { screenVertices[2].x - screenVertices[0].x,
            screenVertices[2].y - screenVertices[0].y,
            0.0f };

       Vector3 cross =  Cross(vec1, vec2);

        // 三角形を描画(背面カリングを考慮)
        if (cross.z > 0) {
            Novice::DrawTriangle(
                int(screenVertices[0].x), int(screenVertices[0].y),
                int(screenVertices[1].x), int(screenVertices[1].y),
                int(screenVertices[2].x), int(screenVertices[2].y),
                RED, kFillModeSolid);
        }

        ///
        /// ↑描画処理ここまで
        ///

        // フレームの終了
        Novice::EndFrame();

        // ESCキーが押されたらループを抜ける
        if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
            break;
        }
    }

    // ライブラリの終了
    Novice::Finalize();
    return 0;
}
