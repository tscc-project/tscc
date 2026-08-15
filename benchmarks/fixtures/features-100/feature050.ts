function id50<T>(x: T): T { return x; }
class Base50<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child50 extends Base50<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E50 { A, B = 3, C, Label = "label" }
const x50: number = id50<number>(50);
const c50 = new Child50("n", x50);
console.log(c50.name, E50.A, E50[3]);
