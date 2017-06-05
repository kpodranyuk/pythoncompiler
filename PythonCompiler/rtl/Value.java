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
    /*** МЕТОДЫ ДЛЯ ХЕШ-ТАБЛИЦ***/
    
//    /**
//     *
//     * @param key
//     * @return
//     */
//    public Value get(Value key) {
//        throw new Error("Operation not allowed with these types.");
//    }
//
//    /**
//     *
//     * @param key
//     * @param val
//     * @return
//     */
//    public Value put(Value key, Value val) {
//        throw new Error("Operation not allowed with these types.");
//    }
//
//    /**
//     *
//     * @param key
//     * @return
//     */
//    public Value remove(Value key) {
//        throw new Error("Operation not allowed with these types.");
//    }

    /**
     * Операция отрицания
     * @return инвертированное значение
     */
    public Value not() {
        throw new Error("Operation not allowed with these types.");
    }

    /**
     * Логическая операция и
     * @param other второе значение
     * @return новое значение
     */
    public Value and(Value other) {
        throw new Error("Operation not allowed with these types.");
    }

    /**
     * Логическая операция или
     * @param other второе значение
     * @return новое значение
     */
    public Value or(Value other) {
        throw new Error("Operation not allowed with these types.");
    }

    /**
     * Операция !=
     * @param other второе значение
     * @return новое значение
     */
    /*public Value neq(Value other) {
        return eq(other).not();
    }*/

    /**
     * Конвертация логического значения в целое число
     * @return логическое значение в виде целого числа
     */
    public int toIntBool() {
        return 0;
    }

    /**
     * Операция ==
     * @param other второе значение
     * @return новое значение
     */
    public Value eq(Value other) {
        return new Boolean(false);
    }

    /**
     * Операция <
     * @param other второе значение
     * @return новое значение
     */
    public Value less(Value other) {
        return new Boolean(false);
    }

    /**
     * Операция >
     * @param other второе значение
     * @return новое значение
     */
    public Value more(Value other) {
        return lessOrEq(other).not();
    }

    /**
     * Операция <=
     * @param other второе значение
     * @return новое значение
     */
    public Value lessOrEq(Value other) {
        return less(other).or(eq(other));
    }

    /**
     * Операция >=
     * @param other второе значение
     * @return новое значение
     */
    public Value moreOrEq(Value other) {
        return less(other).not();
    }

}
