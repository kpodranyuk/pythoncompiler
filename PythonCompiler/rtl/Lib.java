/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package rtl;

import java.io.PrintStream;
import java.util.Scanner;
/**
 * Класс основной библиотеки RTL для использования
 * Служит оберткой для вызовов методов
 * Реализует системные функции (ввод/вывод)
 * @author Katie
 */
public class Lib {
    private static final java.lang.String EXCEPTION_TYPE_MISMATCH = "[ERROR] Type mismatch.";

    public static Value toNumber(Value e) {
        if ((e instanceof Integer)) {   // Целое возвращает себя
            return e;
        }
        if ((e instanceof Float)) {     // Флоат - только целая часть
            return valueFromInt((int)((Float) e).value);
        }
        if ((e instanceof String)) {    // Строка - перевод
            return new Integer(java.lang.Integer.parseInt(((String) e).value));
        }
        if ((e instanceof Boolean)) {   // Булеан - 0 или 1
            return new Integer(toIntBool(((Boolean) e)));
        }
        if ((e instanceof None)) {      // None - 0
            return new Integer(0);
        }
        return valueFromNone();
    }

    public static Value tonumberBase(Value e, Value base) {
        //throw new Error("[ERROR] tonumber: base not supported.");
        /*if ((e instanceof Integer)) {
            return e;
        }*/
        if (!(e instanceof String)) {   // e - может быть только строкой
            throw new Error("Can't use int(x,base) function with first argument that is not a string");
        }
        if(!(base instanceof Integer)){ // base - может быть только целым числом
            throw new Error("Can't use int(x,base) function with second argument that is not an integer");
        }
        // Мы принимаем все основания кроме 0, 1 и больше 36
        int baseI = ((Integer)base).value;
        if(baseI<2||baseI>36)
            throw new Error("Incorrect base in int(x,base) call: "+base.toString());
        if ((e instanceof String)) {
            return new Integer(java.lang.Integer.parseInt(((String) e).value, ((Integer)base).value));
        }
        return valueFromNone();
    }

    

    public static void print(Value val) {
        System.out.println(val.toString());
    }

    public static Value input(Value toPrint) {
        // Печатаем передаваемое содержимое
        if (!(toPrint instanceof None))
            Lib.print(toPrint);
        // Создаем сканнер
        Scanner scan = new Scanner(System.in);
        // Считываем строку
        java.lang.String s = scan.next();
        // Кладем строку на стек
        return valueFromString(s);
    }

    public static Value valueFromNone() {
        return new None();
    }
    
    public static Value valueFromInt(int i) {
        return new Integer(i);
    }

    public static Value valueFromFloat(float f) {
        return new Float(f);
    }

    public static Value valueFromString(java.lang.String s) {
        return new String(s);
    }

    public static Value valueFromList() {
        return new List();
    }
    
    public static Value valueFromBoolean(int val) {
        return new Boolean(val!=0);
    }
    
    public static Value toString(Value e) {
        return valueFromString(e.toString());
    }

    public static Value sub(Value m1, Value m2) {
        return m1.sub(m2);
    }

    public static Value add(Value m1, Value m2) {
        return m1.add(m2);
    }

    public static Value mul(Value m1, Value m2) {
        return m1.mul(m2);
    }
    
    public static Value pow(Value m1, Value m2) {
        return m1.pow(m2);
    }

    public static Value div(Value m1, Value m2) {
        return m1.div(m2);
    }
    
    public static Value intDiv(Value m1, Value m2) {
        return m1.intDiv(m2);
    }

    public static Value mod(Value m1, Value m2) {
        return m1.mod(m2);
    }

    public static int toIntBool(Value m1) {
        return m1.toIntBool();
    }

    public static Value eq(Value m1, Value m2) {
        return m1.eq(m2);
    }

    public static Value less(Value m1, Value m2) {
        return m1.less(m2);
    }

    public static Value more(Value m1, Value m2) {
        return m1.more(m2);
    }

    public static Value lessOrEq(Value m1, Value m2) {
        return m1.lessOrEq(m2);
    }

    public static Value moreOrEq(Value m1, Value m2) {
        return m1.moreOrEq(m2);
    }

    public static Value notEq(Value m1, Value m2) {
        return m1.eq(m2).not();
    }

    public static Value not(Value m) {
        return m.not();
    }

    public static Value or(Value m1, Value m2) {
        return m1.or(m2);
    }

    public static Value and(Value m1, Value m2) {
        return m1.and(m2);
    }

    public static Value ListGet(Value list, Value index) {
        if (((list instanceof List)) && ((index instanceof Integer))) {
            int newIndex = ((Integer) index).value;
            if(newIndex<0) {
                newIndex=flipIndex(newIndex, ((List) list).value.size());
            }
            
            return ((List)list).value.get(newIndex);
        } else if (((list instanceof String)) && ((index instanceof Integer))) {
            int newIndex = ((Integer) index).value;
            if(newIndex<0) {
                newIndex=flipIndex(newIndex, ((String) list).value.length());
            }
            
            return ((String)list).charAt(new Integer(newIndex));
        }
        throw new Error("The index must be an integer and the operation must occur over the array.");
    }

    public static void ListSet(Value list, Value index, Value val) {
        if(!(list instanceof List)){
            throw new Error("No list.");
        } 
        if(!(index instanceof Integer)) {
            throw new Error("Wrong index.");
        }
        
        int newIndex = ((Integer) index).value;
            if(newIndex<0) {
                newIndex=flipIndex(newIndex, ((List) list).value.size());
            }
            ((List) list).value.set(newIndex, val);
    }

    public static Value ListAppend(Value list, Value val) {
        if ((list instanceof List)) {
            ((List) list).value.add(val);
        } else {
            throw new Error("The operation is not applicable to an array.");
        }
        return valueFromNone();
    }
    
    public static Value ListAddInitialize(Value list, Value val) {
        if ((list instanceof List)) {
            ((List) list).value.add(val);
        } else {
            throw new Error("The operation is not applicable to an array.");
        }
        return list;
    }
    
    public static Value ListRemove(Value list, Value val) {
        if ((list instanceof List)) {
            if(val instanceof None || val instanceof Integer || val instanceof Boolean || val instanceof String || val instanceof Float) {
               ((List) list).value.remove(val); 
            } else {
                throw new Error("Value must be a standard type.");
            }
        } else {
            throw new Error("The operation is not applicable to an array.");
        }
        return valueFromNone();
    }
    
    public static int flipIndex(int index, int size) {
        int newIndex = index;
        newIndex+=size;
        return newIndex;
    }
}
