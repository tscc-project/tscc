namespace N47 {
  export const base: number = 47;
  export function add(x: number): number { return base + x; }
}
function over47(x: number): number;
function over47(x: string): string;
function over47(x: number | string): number | string { return x; }
abstract class A47 { abstract f(x: number): number; }
class B47 extends A47 { override f(x: number): number { return x + 1; } }
enum E47 { A = 5 / 2, B, C = 2 ** 3, D }
const angle47 = <number>(47 + 1);
const check47 = {x: angle47} satisfies {x: number};
console.log(N47.add(1), over47(47), new B47().f(1), E47.B, E47.D, check47.x);
