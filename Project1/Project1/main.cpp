#include "DxLib.h"
#include <math.h>
#include "Distortion.h"

#define SCREEN_W	640			// 画面の幅
#define SCREEN_H	480			// 画面の高さ
constexpr int LineColor = 128;	// ラインの色
constexpr int LineSplit = 16;	// ラインの分割数


Distortion dist;

// 画面を歪ませながら描画する関数


// WinMain 関数
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	int screenHandle_;
	int grHandle_;
	int graphX_, graphAddX_;
	int circleX_, circleAddX_;
	int circleAngle_;
	int line_;



	// ウインドウモードで起動
	ChangeWindowMode(TRUE);

	// ＤＸライブラリの初期化
	if (DxLib_Init() < 0) return -1;

	// 描画可能画像の作成
	screenHandle_ = MakeScreen(SCREEN_W, SCREEN_H);

	// 画像の読み込み
	//GrHandle = LoadGraph("Check01.png");
	grHandle_ = LoadGraph("Yakiniku.png");

	// 描画先を裏画面にする
	SetDrawScreen(DX_SCREEN_BACK);

	// 画像と円の推移用パラメータを初期化
	graphX_ = 0;
	graphAddX_ = 0;
	circleX_ = 320;
	circleAddX_ = 0;
	circleAngle_ = 0;

	// メインループ(何かキーが押されたらループを抜ける)
	while (ProcessMessage() == 0&&!CheckHitKey(KEY_INPUT_ESCAPE))
	{
		// 画像を移動
		graphX_ += graphAddX_;
		if (graphX_ < 0 || graphX_ > 640 - 119) graphAddX_ = -graphAddX_;

		// 歪み円を移動
		circleX_ += circleAddX_;
		if (circleX_ < 0 || circleX_ > 640) circleAddX_ = -circleAddX_;

		// 歪み円の円周の値を揺らがせる処理
		circleAngle_ += 1;
		if (circleAngle_ >= 360) circleAngle_ = 0;

		// 描画先を描画可能画像にする
		SetDrawScreen(screenHandle_);

		// 画面のクリア
		ClearDrawScreen();

		// 画面に格子を描画する
		for (line_ = 0; line_ < SCREEN_W / LineSplit; line_++)
			DrawLine(line_ * LineSplit + 1, 0, line_ * LineSplit + 1, SCREEN_H, GetColor(LineColor, LineColor, LineColor));

		for (line_ = 0; line_ < SCREEN_H / LineSplit; line_++)
			DrawLine(0, line_ * LineSplit + 1, SCREEN_W, line_ * LineSplit + 1, GetColor(LineColor, LineColor, LineColor));

		// 画像の描画
		DrawGraph(graphX_, 0, grHandle_, TRUE);

		// 描画先を裏画面にする
		SetDrawScreen(DX_SCREEN_BACK);

		// 画面を歪ませて描画
		dist.DrawCircleScreen(
			circleX_, 240,	// 中心座標
			80.0 + sin(circleAngle_ * PHI_F / 180.0f) * 15.0f,	// 内側の円のサイズ
			200.0 + sin(circleAngle_ * 2 * PHI_F / 180.0f) * 50.0f,	// 外側の円のサイズ
			48.0f,	// 内側に引き込まれるドット数
			screenHandle_);

		// 裏画面の内容を表画面に反映
		ScreenFlip();
	}

	// ＤＸライブラリの後始末
	DxLib_End();

	// ソフトの終了
	return 0;
}