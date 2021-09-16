#pragma once
#ifndef _keyword
#define _keyword

#define PURE			// 대입 연산을 하지 않으며, 결과값이 외부 상태와 무관한 순수 함수
#define NOASSIGNMENT	// 대입 연산을 하지 않는 함수
#define HELPER			// 사용자 코드에서의 직접 호출되지 않고 다른 클래스나 함수의 도우미로써 이용되는 클래스나 함수
#define NONCREATABLE	// 생성이 불가능한 클래스
#define NONCOPYABLE		// 복사가 불가능한 클래스
#define MOVABLE			// 이동 연산을 지원하여, r-value optimization이 가능한 클래스
#define INTERFACE       // 순수 가상함수를 포함하거나, CRTP 패턴에 쓰여 사용자 코드에서 실제 생성할 일이 없고 상속을 위해서만 존재하는 클래스

#endif