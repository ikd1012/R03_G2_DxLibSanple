//�w�b�_�t�@�C���̓ǂݍ���
#include "DxLib.h"	//DxLib���g���Ƃ��͕K�v
#include "keyboard.h"	//�L�[�{�[�h�̏���


//�}�N����`
#define GAME_TITLE "�Q�[���^�C�g��"		//�Q�[���^�C�g��
#define GAME_WIDTH 1280					//�Q�[����ʂ̕�(�E�B�h�X)
#define GAME_HEIGHT 720					//�Q�[����ʂ̍���(�n�C�g)
#define GAME_COLOR 32					//�Q�[���̐F��

#define GAME_ICON_ID 333				//�Q�[����ICON��ID

#define GAME_WINDOW_BAR	0				//�E�B���h�E�o�[�̏��

// �v���O������ WinMain ����n�܂�܂�
//Windows�̃v���O���~���O���@=(WinAPI)�œ����Ă���I
//DxLib�́ADirectX�Ƃ����A�Q�[���v���O���~���O�����񂽂�Ɏg����d�g��
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	SetOutApplicationLogValidFlag(FALSE);					//Log.txt���o�͂��Ȃ�
	ChangeWindowMode(TRUE);									//�E�B���h�E���[�h�ɐݒ�
	SetMainWindowText(GAME_TITLE);							//�E�B���h�E�̃^�C�g���̕���
	SetGraphMode(GAME_WIDTH, GAME_HEIGHT, GAME_COLOR);		//�E�B���h�E�̉𑜓x��ݒ�
	SetWindowSize(GAME_WIDTH, GAME_HEIGHT);					//�E�B���h�E�̑傫����ݒ�
	SetBackgroundColor(255, 255, 255);						//�f�t�H���g�̔w�i�̐F
	SetWindowIconID(GAME_ICON_ID);							//�A�C�R���t�@�C����ǂݍ���
	SetWindowStyleMode(GAME_WINDOW_BAR);					//�E�B���h�E�o�[�̏��
	SetWaitVSyncFlag(TRUE);									//�f�B�X�v���C�̐���������L���ɂ���
	SetAlwaysRunFlag(TRUE);									//�E�B���h�E�������ƃA�N�e�B�u�ɂ���


	if (DxLib_Init() == -1)		// �c�w���C�u��������������
	{
		return -1;				// �G���[���N�����璼���ɏI��
	}
	//�_�u���o�b�t�@�����O�L����
	SetDrawScreen(DX_SCREEN_BACK);

	//�~�̒��S�_
	int X = GAME_WIDTH / 2;
	int Y = GAME_HEIGHT / 2;
	//�~�̔��a
	int radius = 60;

	int speed = 10;
	//�������[�v
	while (1)
	{

		if (ProcessMessage() != 0) { break; }	//���b�Z�[�W���󂯎�葱����
		if (ClearDrawScreen() != 0) { break; }	//��ʂ��폜����

		//�L�[�{�[�h���͂̍X�V
		AllKeyUpdate();
		//�L�[����
		if (KeyDown(KEY_INPUT_W) == TRUE)
		{
			Y--;		//��Ɉړ�
			if (KeyDown(KEY_INPUT_LSHIFT) == TRUE || KeyDown(KEY_INPUT_RSHIFT) == TRUE)
			{
				Y -= speed;
			}
		}
		if (KeyDown(KEY_INPUT_S) == TRUE)
		{
			Y++;		//���Ɉړ�
			if (KeyDown(KEY_INPUT_LSHIFT) == TRUE || KeyDown(KEY_INPUT_RSHIFT) == TRUE)
			{
				Y += speed;
			}
		}
		if (KeyDown(KEY_INPUT_A) == TRUE)
		{
			X--;		//���Ɉړ�
			if (KeyDown(KEY_INPUT_LSHIFT) == TRUE || KeyDown(KEY_INPUT_RSHIFT) == TRUE)
			{
				X -= speed;
			}
		}
		if (KeyDown(KEY_INPUT_D) == TRUE)
		{
			X++;		//�E�Ɉړ�
			if (KeyDown(KEY_INPUT_LSHIFT) == TRUE || KeyDown(KEY_INPUT_RSHIFT) == TRUE)
			{
				X += speed;
			}
		}

		DrawCircle(X, Y, radius, GetColor(255, 255, 0), TRUE);
		ScreenFlip();		//�_�u���o�b�t�@�����O������ʂ�`��
	}
	// �c�w���C�u�����g�p�̏I������
		DxLib_End();

		return 0;				// �\�t�g�̏I�� 
}