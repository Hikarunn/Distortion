#include "DxLib.h"
#include <math.h>
#include "Distortion.h"

#define SCREEN_W	640			// ��ʂ̕�
#define SCREEN_H	480			// ��ʂ̍���
constexpr int LineColor = 128;	// ���C���̐F
constexpr int LineSplit = 16;	// ���C���̕�����


Distortion dist;

// ��ʂ�c�܂��Ȃ���`�悷��֐�


// WinMain �֐�
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	int screenHandle_;
	int grHandle_;
	int graphX_, graphAddX_;
	int circleX_, circleAddX_;
	int circleAngle_;
	int line_;



	// �E�C���h�E���[�h�ŋN��
	ChangeWindowMode(TRUE);

	// �c�w���C�u�����̏�����
	if (DxLib_Init() < 0) return -1;

	// �`��\�摜�̍쐬
	screenHandle_ = MakeScreen(SCREEN_W, SCREEN_H);

	// �摜�̓ǂݍ���
	//GrHandle = LoadGraph("Check01.png");
	grHandle_ = LoadGraph("Yakiniku.png");

	// �`���𗠉�ʂɂ���
	SetDrawScreen(DX_SCREEN_BACK);

	// �摜�Ɖ~�̐��ڗp�p�����[�^��������
	graphX_ = 0;
	graphAddX_ = 0;
	circleX_ = 320;
	circleAddX_ = 0;
	circleAngle_ = 0;

	// ���C�����[�v(�����L�[�������ꂽ�烋�[�v�𔲂���)
	while (ProcessMessage() == 0&&!CheckHitKey(KEY_INPUT_ESCAPE))
	{
		// �摜���ړ�
		graphX_ += graphAddX_;
		if (graphX_ < 0 || graphX_ > 640 - 119) graphAddX_ = -graphAddX_;

		// �c�݉~���ړ�
		circleX_ += circleAddX_;
		if (circleX_ < 0 || circleX_ > 640) circleAddX_ = -circleAddX_;

		// �c�݉~�̉~���̒l��h�炪���鏈��
		circleAngle_ += 1;
		if (circleAngle_ >= 360) circleAngle_ = 0;

		// �`����`��\�摜�ɂ���
		SetDrawScreen(screenHandle_);

		// ��ʂ̃N���A
		ClearDrawScreen();

		// ��ʂɊi�q��`�悷��
		for (line_ = 0; line_ < SCREEN_W / LineSplit; line_++)
			DrawLine(line_ * LineSplit + 1, 0, line_ * LineSplit + 1, SCREEN_H, GetColor(LineColor, LineColor, LineColor));

		for (line_ = 0; line_ < SCREEN_H / LineSplit; line_++)
			DrawLine(0, line_ * LineSplit + 1, SCREEN_W, line_ * LineSplit + 1, GetColor(LineColor, LineColor, LineColor));

		// �摜�̕`��
		DrawGraph(graphX_, 0, grHandle_, TRUE);

		// �`���𗠉�ʂɂ���
		SetDrawScreen(DX_SCREEN_BACK);

		// ��ʂ�c�܂��ĕ`��
		dist.DrawCircleScreen(
			circleX_, 240,	// ���S���W
			80.0 + sin(circleAngle_ * PHI_F / 180.0f) * 15.0f,	// �����̉~�̃T�C�Y
			200.0 + sin(circleAngle_ * 2 * PHI_F / 180.0f) * 50.0f,	// �O���̉~�̃T�C�Y
			48.0f,	// �����Ɉ������܂��h�b�g��
			screenHandle_);

		// ����ʂ̓��e��\��ʂɔ��f
		ScreenFlip();
	}

	// �c�w���C�u�����̌�n��
	DxLib_End();

	// �\�t�g�̏I��
	return 0;
}