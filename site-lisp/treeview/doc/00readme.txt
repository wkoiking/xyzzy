-*- outline-tree: topic -*-
======================================================================
                treeview  -- TreeView.dll �p���C�u����
                --------------------------------------

    Copyright (C) 2001-2006 OHKUBO Hiroshi <ohkubo@s53.xrea.com>

    Author: OHKUBO Hiroshi <ohkubo@s53.xrea.com>
    Time-stamp: <2006/01/16 23:28:32 +0900>
======================================================================

���T�v

  kazu.y ����� TreeView.dll �𕡐��ړI�Ɏg�p���邽�߂̃��C�u������
  ���B

  �d�l�� FIX ���Ă��܂���̂ŁA���̃��C�u�����Ɉˑ������R�[�h�̌��J
  �̍ۂ͂����ӂ��������B

���C���X�g�[��

  0. �K�v�ȃ��C�u�����𓱓����܂��B

          - TreeView.dll (ver. 1.03 �ȍ~)
          - color
          - win-window

  1. �A�[�J�C�u��W�J���� treeview/ �ȉ��� $XYZZY/site-lisp ��
     �R�s�[���܂��B

  2. �K�v�Ȃ�΃o�C�g�R���p�C�����܂��B

          M-x load-library
          Load library: treeview/makefile

          M-x treeview-make-clean
          M-x treeview-make-all

  3. ~/.xyzzy �܂��� $XYZZY/site-lisp/siteinit.l �Ɉȉ��̃R�[�h��
     �ǉ����܂��B

          (require "treeview/setup")

  4. ��L�̐ݒ�𔽉f�����邽�߂ɁAxyzzy ���ċN�����܂��B
     siteinit.l �ɋL�q�����ꍇ�� Ctrl �L�[�� Shift �L�[�������Ȃ���
     xyzzy ���ċN�����A�_���v�t�@�C�����č\�z���܂��B

���A���C���X�g�[��

  1. treeview ���C�u�����Ɉˑ����镔����S�� xyzzy ������
     ���O���܂��B

  2. ESC ESC (treeview::treeview-uninstall) �ƃ^�C�v���A
     treeview �֘A�̏��� xyzzy ����폜���܂��B

  3. treeview �Ɋւ���L�q���폜���܂��B

  4. siteinit.l �ɋL�q���Ă����ꍇ�� Ctrl �L�[�� Shift �L�[������
     �Ȃ��� xyzzy ���ċN�����A�_���v�t�@�C�����č\�z���܂��B

�����C�Z���X

  treeview �͏C��BSD���C�Z���X�Ɋ�Â��ė��p�\�ł��B
  <http://www.opensource.org/licenses/bsd-license.php>


  Copyright (C) 2001-2006, OHKUBO Hiroshi.  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

  1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.

  3. Neither the name of the University nor the names of its
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

���g�킹�Ē����Ă������

  �Ekazu.y ����� TreeView.dll
    http://www003.upp.so-net.ne.jp/rex/

