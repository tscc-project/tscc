function id6<T>(x: T): T { return x; }
class Base6<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child6 extends Base6<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E6 { A, B = 3, C, Label = "label" }
const x6: number = id6<number>(6);
const c6 = new Child6("n", x6);
console.log(c6.name, E6.A, E6[3]);
