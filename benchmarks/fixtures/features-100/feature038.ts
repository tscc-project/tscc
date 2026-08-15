function id38<T>(x: T): T { return x; }
class Base38<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child38 extends Base38<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E38 { A, B = 3, C, Label = "label" }
const x38: number = id38<number>(38);
const c38 = new Child38("n", x38);
console.log(c38.name, E38.A, E38[3]);
