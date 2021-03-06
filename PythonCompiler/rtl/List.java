/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package rtl;

import java.util.ArrayList;

/**
 *
 * @author Никита
 */
public class List extends Value {

    public ArrayList<Value> value;
    
    public List() {
        this.value = new ArrayList<Value>();
    }
  
    @Override
    public Value add(Value other) {
        if(other instanceof List) {
            List newList = new List();
            newList.value.addAll(this.value);
            newList.value.addAll(((List) other).value);
        
            return newList;
        }
        throw new Error("Operation not allowed with these types.");
    }

    @Override
    public Value mul(Value other) {
        if(other instanceof Integer) {
            int val=((Integer) other).value;
            if(val<=0) {
                return new List();
            } else if(val==1) {
                List list = new List();
                list.value.addAll(this.value);
                return list;
            } else {
                List list = new List();
                for(int i=0; i<val;i++) {
                    list.value.addAll(this.value);
                }
                return list;
            }
        } else {
            throw new Error("Operation not allowed with these types.");
        }
    }
    

    @Override
    public int toIntBool() {
        if(this.value.size() == 0)
            return 0;
        return 1;
    }
  
    @Override
    public Value less(Value other) {
        if(other instanceof List) {
            List list = (List)other;
            int sizeList;
            // Находим наименьший из листов(размер)
            if(this.value.size() > list.value.size()) {
                sizeList=list.value.size();
            } else {
                sizeList=this.value.size();
            }
            
            // Если листы равны, то результатом операции будет false
            if(this.equals(list)) {
                return new Boolean(false);
            }
            
            // Проходим по всем элементам наименьшего листа
            // Результатом лог выражения будет первое значение true или false
            for(int i=0; i<sizeList; i++) {
                if((this.value.get(i).less(list.value.get(i))).toIntBool()==1 && !this.value.get(i).equals(list.value.get(i))) {
                    return new Boolean(true);
                } else if ((this.value.get(i).less(list.value.get(i))).toIntBool()==0 && !this.value.get(i).equals(list.value.get(i)))
                    return new Boolean(false);
            }
            
            if(this.value.size() < list.value.size()) {
                return new Boolean(true);
            } else {
                return new Boolean(false);
            }
            
        } else {
            throw new Error("Operation not allowed with these types.");
        }
    }
    
    
    @Override
    public boolean equals(Object paramObject) {
        Value obj = (Value) paramObject;
        if(obj instanceof List) {
            List list = (List) obj;
            if(list.value.size() != this.value.size()) {
                return false;
            } else {
                for(int i=0; i<list.value.size(); i++) {
                    if(!list.value.get(i).equals(this.value.get(i)))
                        return false;
                }
                return true;
            }
        }
        return false;
    }

    @Override
    public java.lang.String toString() {
        java.lang.String res = "[ ";
        for (int i=0; i<this.value.size(); i++) {
          res = res + this.value.get(i).toString();
          if(i!=this.value.size()-1) {
              res = res + ", ";
          }
        }
        return res + "]";
    }

    @Override
    public Value clone() {
        List list = new List();
        list.value.addAll(this.value);
        return list;
    }

    @Override
    public int hashCode() {
        return 0;
    }
    
}
