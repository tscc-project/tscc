
function wrap<T extends {value: unknown}>(x: T): T { return x; }
const genericArrow = <T>(x: T): T => x;
const nested: Promise<Array<string>> | null = null;
const value = genericArrow<Array<string>>(["a","b"]);
class Base<T> { constructor(public value: T) {} }
class Derived<T extends string> extends Base<Array<T>> {
  method<U extends Array<T>>(x: U): U { return x; }
}
console.log(genericArrow<number>(3), value[1], new Derived<string>(["x"]).value[0]);
