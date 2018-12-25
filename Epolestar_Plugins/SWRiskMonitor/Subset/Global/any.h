#pragma once

#include <typeinfo>


//�Զ����any��  
class any  
{  
private:        
    //�����������ݵĽӿ���  
    class placeholder  
    {  
    public:       
        virtual ~placeholder(){}    
        virtual const std::type_info & type() const = 0;  
        virtual placeholder * clone() const = 0; 
    };  
  
    //��������ͻ�ȡ���ݵ��ࡣ  
    template<typename _ty>
	class holder : public placeholder  
    {  
	private:
		holder & operator=(const holder &);
    public:           
        _ty m_value;						//���������ݣ��ͱ��������� 

        holder(const _ty & _value): m_value(_value){}  
        virtual const std::type_info & type() const  
        {  
            return typeid(_ty);  
        }    
        virtual placeholder * clone() const  
        {  
            return new holder(m_value);		//ԭ��ģʽ  
        }   
    };    
private:  
    //һ��placeholde����ָ�룬ָ��������holder��һ��ʵ��,��m_pcontent(new holder<_ty>(value))���  
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
		any(_any).swap(*this);				//�½�anyʵ��Ȼ�󽻻�����ֹ�ı�_any��ֵ
		return *this;
	}  
	template<typename _ty> 
	any & operator=(const _ty & _value)
	{
		any(_value).swap(*this);
		return *this;
	}   	
public:   
    //��ѯ��ʵ���ݵ����͡�  
    const std::type_info & type() const  
    {  
        return m_pcontent ? m_pcontent->type() : typeid(void);  
    }   	
	bool empty() const
	{
		return m_pcontent == nullptr;
	}
	
	//���ش洢��ֵ
	template<typename _ty>
	_ty * value_ptr() const
	{
		if (type() != typeid(_ty))
			throw(1);
		return &static_cast<any::holder<_ty> *>(m_pcontent)->m_value;	//reinterpret_cast
	}
	//���ش洢��ֵ
	template<typename _ty>
	_ty & value() const
	{
		if (type() != typeid(_ty))
			throw(1);
		return static_cast<any::holder<_ty> *>(m_pcontent)->m_value;	//reinterpret_cast
	}
	//���ش洢��ֵ��_value
	template<typename _ty>
	bool value(_ty & _value) const
	{
		if (type() != typeid(_ty))
			return false;
		_value = value<_ty>();
		return true;
	}
};