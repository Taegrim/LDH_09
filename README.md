# 숫자 야구 프로젝트

## 구현 기능

- 서버에서 중복 없는 3자리 정답 숫자 생성
- 채팅 입력을 이용한 숫자 야구 추측 처리
- Strike / Ball / OUT 판정
- PlayerState를 이용한 플레이어별 시도 횟수 관리
- GameMode에서 승리, 무승부, 게임 리셋 처리
- 접속 순서 기반 턴제 진행
- GameState를 이용한 남은 시간 및 현재 턴 정보 동기화
- 제한 시간 초과 시 현재 턴 플레이어의 기회 차감하고 턴 넘어가도록 함
- Client RPC를 이용한 채팅 및 알림 메시지 출력
- Client RPC + DELEGATE, OnRep + DELEGATE 를 이용한 UI 연동
