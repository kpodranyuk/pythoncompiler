/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package rtl;

/**
 * Класс динамической типизации Абстрактен
 *
 * @author Katie
 */
public abstract class Value {

    /*
    * Это - абстрактные методы, которые ОБЯЗАТЕЛЬНО должны быть реализованы
    * так как класс, по идее наследуется от класса Object
    */
    @Override
    public abstract boolean equals(Object paramObject);

    @Override
    public abstract java.lang.String toString();

    @Override
    public abstract Value clone();

    @Override
    public abstract int hashCode();
    
    /**
     * Конвертация логического значения в целое число
     * @return логическое значение в виде целого числа
     */
    public int toIntBool() {
        return 0;
    }

    /**
     * Прибавляет к первому значению второе
     * @param other второе значение
     * @return новое значение
     */
    public Value add(Value other) {
        throw new Error("Operation not allowed with these types.");
    }

    /**
     * Отнимает от первого значения второе
     * @param other второе значение
     * @return новое значение
     */
    public Value sub(Value other) {
        throw new Error("Operation not allowed with these types.");
    }

    /**
     * Умножает первое значение на второе
     * @param other второе значение
     * @return новое значение
     */
    public Value mul(Value other) {
        throw new Error("Operation not allowed with these types.");
    }

    /**
     * Делит первое значение на второе
     * @param other второе значение
     * @return новое значение
     */
    public Value div(Value other) {
        throw new Error("Operation not allowed with these types.");
    }

    /**
     * Получает остаток от деления первого значения на второе
     * @param other второе значение
     * @return новое значение
     */
    public Value mod(Value other) {
        throw new Error("Operation not allowed with these types.");
    }

    /**
     * Операция отрицания
     * @return инвертированное значение
     */
    public Value not() {
        if(this.toIntBool()==0) {
            return new Boolean(true);
        }
        return new Boolean(false);
    }

    /**
     * Логическая операция и
     * @param other второе значение
     * @return новое значение
     */
    public Value and(Value other) {
        if(this.toIntBool()==1) {
            return other;
        }
        return this;
    }

    /**
     * Логическая операция или
     * @param other второе значение
     * @return новое значение
     */
    public Value or(Value other) {
        if(this.toIntBool()==1) {
            return this;
        }
        return other;
    }
    

    /**
     * Операция ==
     * @param other второе значение
     * @return новое значение
     */
    public Value eq(Value other) {
        if(this.equals(other)) {
            return new Boolean(true);
        }
        return new Boolean(false);
    }

    /**
     * Операция <
     * @param other второе значение
     * @return новое значение
     */
    public Value less(Value other) {
        return new Boolean(false);// TO DO вставить исключение
    }

    /**
     * Операция >
     * @param other второе значение
     * @return новое значение
     */
    public Value more(Value other) {
        return lessOrEq(other).not();// TO DO вставить исключение
    }

    /**
     * Операция <=
     * @param other второе значение
     * @return новое значение
     */
    public Value lessOrEq(Value other) {
        return less(other).or(eq(other));// TO DO вставить исключение
    }

    /**
     * Операция >=
     * @param other второе значение
     * @return новое значение
     */
    public Value moreOrEq(Value other) {
        return less(other).not();// TO DO вставить исключение
    }

}
