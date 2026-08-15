function id59<T>(x: T): T { return x; }
class Base59<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child59 extends Base59<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E59 { A, B = 3, C, Label = "label" }
const x59: number = id59<number>(59);
const c59 = new Child59("n", x59);
console.log(c59.name, E59.A, E59[3]);
