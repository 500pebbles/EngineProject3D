#pragma once

#include <Core/Core.h>
#include <memory>

// 단일 상속만 허용하지만, 이는 같은 UObject에서 파생받는 부모가 하나 (인터페이스나 ASC나 이런건 ok)
class ENGINE_API UObject
{
public:
    // 다형성 클래스를 위한 가상 소멸자
    virtual ~UObject() = default;

    // 현재 객체의 타입ID를 반환
    virtual size_t GetType() const = 0;

    // 파라미터로 들어온 객체의 ID와 같은 타입, 혹은 상속받은 타입인지 검사
    virtual bool Is(size_t id) const
    {
        return false; // 최상위 UObject까지 재귀로 왔다는건 이미 같은 타입이나 상속받은 타입 아니라는 것
    }

    // 타입 질의함수
    template<typename T>
    bool IsTypeOf() const
    {
        return Is(T::TypeId());
    }
};    

// 상속받은 관계간의 자동 형변환 함수. dynamic_cast 대신 자체적으로 검사 체크후 가벼운 static_cast
template<typename T, typename U>
std::shared_ptr<T> Cast(const std::shared_ptr<U>& object)
{
    if (!object) return nullptr; // 파라미터가 유효하지 않다면 nullptr 리턴            
    if (object->Is(T::TypeId())) return std::static_pointer_cast<T>(object); // 상속관계가 확인되면 정적 캐스팅   
    return nullptr;             // 상속관계가 없다면 실패, nullptr 리턴

}

/* UObject 를 상속받는 모든 클래스는 virtual 가상함수들을 재정의 해야하는데
 * 모두 동일한 기능을 하는 타입체크 관련 반복해서 재정의하면 헤더파일이 장황해짐
 * 언리얼의 GENERATED_BODY()의 역할과 유사한 매크로 삽입문 */
#define TYPE_DECLARATIONS(Type, ParentType)                          \
using super = ParentType;                                            \
                                                                     \
protected:                                                           \
    /* 정적 변수는 모든 객체가 공유하므로, 고유한 주소값으로 ID를 반환받음 */   \
    static size_t TypeIdClass()                                      \
    {                                                                \
        static int runTimeTypeId = 0;                                \
        return reinterpret_cast<size_t>(&runTimeTypeId);             \
    }                                                                \
                                                                     \
public:                                                              \
    /* (정적) 위 TypeIdClass()의 runTimeTypeId를 받아오는 Getter 기능 */ \
    static size_t TypeId()                                           \
    {                                                                \
        return Type::TypeIdClass();                                  \
    }                                                                \
    /* 위와 같은 기능인데 객체단위의 반환. 이건 객체의 타입, 위는 검사용  */   \
    virtual size_t GetType() const override                          \
    {                                                                \
        return Type::TypeIdClass();                                  \
    }                                                                \
    /* 같은 타입인지 검사하고, 아니면 부모까지 올라가는 재귀형식의 검사문 */    \
    virtual bool Is(size_t id) const override                        \
    {                                                                \
        return (id == TypeIdClass()) ? true : ParentType::Is(id);    \
    }																
