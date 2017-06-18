/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package rtl;

import java.util.Stack;

/**
 *
 * @author Никита
 */
public class Iterator extends Value {

    public Value currentValue; // Текущее значение итератора
    public Stack<Value> values; // Списки или строки по которым идет итератор
    public Stack<java.lang.Integer> indexes; // Текущие индексы итератора
    public boolean exist = false; // Итератор существует
    
    public Iterator() {
        this.exist = true;
        this.values = new Stack<Value>();
        this.indexes = new Stack<java.lang.Integer>();
    }
    
    
    public int hasNext() {
        int has=0;
        //System.out.println("Зашел в метод hasNext в классе Iterator");
        if(!this.values.empty()) {
            //System.out.println("Зашел в метод hasNext в классе Iterator-векта ифа");
            Value list = this.values.peek();
            int index = this.indexes.peek();
            //System.out.println(list+"  "+index);
            int size = 0;
            if(list instanceof List) {
                size=((List)list).value.size();
            } else {
                size=((String)list).value.length();
            }
            //System.out.println(size+" - размер листа");
            
            if(size > index) {
                has=1; 
            } else {
                //System.out.println(size+"Зашел в метод hasNext в классе Iterator-векта елсе при сравнивании индекса и размера");
                has=0;
                this.indexes.pop();
                this.values.pop();
            }
        }
        
        return has;
    }
    
    public Value next() {
        Value res = null;
        //System.out.println("Зашел в метод next в классе Iterator");
        if(!this.values.empty()) {
            //System.out.println("Зашел в метод next в классе Iterator-ветка ифа");
            Value list = this.values.peek();
            int index = this.indexes.peek();
            if(list instanceof List) {
                //System.out.println("Зашел в метод next в классе Iterator-ветка ифа, получение значения");
                res = ((List)list).value.get(index);
                currentValue = ((List)list).value.get(index);
            } else if(list instanceof String) {
                res = ((String)list).charAt(new Integer(index+1));
                currentValue = ((String)list).charAt(new Integer(index+1));
            }
            //System.out.println("Зашел в метод next в классе Iterator-ветка ифа, попаем индекс");
            this.indexes.pop();
            this.indexes.push(index+1);// Кладем следующий индекс
        }
        
        return res;
    }
    
    public Value newIterator(Value listOrStr) {
        this.values.push(listOrStr);
        this.indexes.push(0);
        //System.out.println("Зашел в метод newIterator в классе Iterator");
        return this;
    }
    
    
    @Override
    public boolean equals(Object paramObject) {
        Value obj = (Value) paramObject;
        return currentValue.equals(obj);
    }

    @Override
    public java.lang.String toString() {
        return currentValue.toString();
    }

    @Override
    public Value clone() {
        return currentValue.clone();
    }

    @Override
    public int hashCode() {
        return currentValue.hashCode();
    }
    
    
}
