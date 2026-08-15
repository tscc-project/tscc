namespace N33 {
  export const base: number = 33;
  export function add(x: number): number { return base + x; }
}
function over33(x: number): number;
function over33(x: string): string;
function over33(x: number | string): number | string { return x; }
abstract class A33 { abstract f(x: number): number; }
class B33 extends A33 { override f(x: number): number { return x + 1; } }
enum E33 { A = 5 / 2, B, C = 2 ** 3, D }
const angle33 = <number>(33 + 1);
const check33 = {x: angle33} satisfies {x: number};
console.log(N33.add(1), over33(33), new B33().f(1), E33.B, E33.D, check33.x);
