function id11<T>(x: T): T { return x; }
class Base11<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child11 extends Base11<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E11 { A, B = 3, C, Label = "label" }
const x11: number = id11<number>(11);
const c11 = new Child11("n", x11);
console.log(c11.name, E11.A, E11[3]);
