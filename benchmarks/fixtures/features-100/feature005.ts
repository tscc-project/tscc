function id5<T>(x: T): T { return x; }
class Base5<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child5 extends Base5<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E5 { A, B = 3, C, Label = "label" }
const x5: number = id5<number>(5);
const c5 = new Child5("n", x5);
console.log(c5.name, E5.A, E5[3]);
