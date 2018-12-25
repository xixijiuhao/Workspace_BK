#pragma once

#include <typeinfo>


//自定义的any类  
class any  
{  
private:        
    //保存真正数据的接口类  
    class placeholder  
    {  
    public:       
        virtual ~placeholder(){}    
        virtual const std::type_info & type() const = 0;  
        virtual placeholder * clone() const = 0; 
    };  
  
    //真正保存和获取数据的类。  
    template<typename _ty>
	class holder : public placeholder  
    {  
	private:
		holder & operator=(const holder &);
    public:           
        _ty m_value;						//真正的数据，就保存在这里 

        holder(const _ty & _value): m_value(_value){}  
        virtual const std::type_info & type() const  
        {  
            return typeid(_ty);  
        }    
        virtual placeholder * clone() const  
        {  
            return new holder(m_value);		//原型模式  
        }   
    };    
private:  
    //一个placeholde对象指针，指向其子类holder的一个实现,即m_pcontent(new holder<_ty>(value))语句  
    placeholder * m_pcontent;  
public:    
    any(): m_pcontent(nullptr)
	{}  
    template<typename _ty>  
    any(const _ty & _value): m_pcontent(new holder<_ty>(_value))
	{}  
    any(const any & _any): m_pcontent(_any.m_pcontent ? _any.m_pcontent->clone() : nullptr)
	{}    
    virtual ~any()  
    {  
        if(nullptr != m_pcontent)  
            delete m_pcontent;  
    }     
public:  
	any & swap(any & _any)
	{
		placeholder * tmp	= m_pcontent;
		m_pcontent			= _any.m_pcontent;
		_any.m_pcontent		= tmp;

		return *this;
	}  
	any & operator=(const any & _any)
	{
		any(_any).swap(*this);				//新建any实例然后交换，防止改变_any的值
		return *this;
	}  
	template<typename _ty> 
	any & operator=(const _ty & _value)
	{
		any(_value).swap(*this);
		return *this;
	}   	
public:   
    //查询真实数据的类型。  
    const std::type_info & type() const  
    {  
        return m_pcontent ? m_pcontent->type() : typeid(void);  
    }   	
	bool empty() const
	{
		return m_pcontent == nullptr;
	}
	
	//返回存储的值
	template<typename _ty>
	_ty * value_ptr() const
	{
		if (type() != typeid(_ty))
			throw(1);
		return &static_cast<any::holder<_ty> *>(m_pcontent)->m_value;	//reinterpret_cast
	}
	//返回存储的值
	template<typename _ty>
	_ty & value() const
	{
		if (type() != typeid(_ty))
			throw(1);
		return static_cast<any::holder<_ty> *>(m_pcontent)->m_value;	//reinterpret_cast
	}
	//返回存储的值到_value
	template<typename _ty>
	bool value(_ty & _value) const
	{
		if (type() != typeid(_ty))
			return false;
		_value = value<_ty>();
		return true;
	}
};