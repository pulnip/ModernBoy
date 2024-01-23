#pragma once

#include "Makable.hpp"
#include "Observable.hpp"

enum class Lifetime {
    CONSTRUCTED,
    DESTRUCTED
};

template<typename Base, typename Owner>
class Owned:
    public Makable<Base, Owner>,
    public UniqueObservable<Lifetime, Base>
{
  public:
    virtual ~Owned(){
        if(Makable<Base, Owner>::owner.expired()) return;
        UniqueObservable<Lifetime, Base>::notify(Lifetime::DESTRUCTED);
    }

  protected:
    virtual void postConstruct() noexcept override final{
        UniqueObservable<Lifetime, Base>::subscribe(
            Makable<Base, Owner>::owner
        );
        UniqueObservable<Lifetime, Base>::notify(Lifetime::CONSTRUCTED);
        
        setProperty();
    }

  private:
    virtual void setProperty() noexcept=0;
};

template<typename Owned>
class Owner:
    public Observer<Lifetime, Owned>
{
  private:
    virtual void onNotify(
        Lifetime msg, std::shared_ptr<Owned> owned
    ) noexcept override=0;
};
