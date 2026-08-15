namespace N88 {
  export const base: number = 88;
  export function add(x: number): number { return base + x; }
}
function over88(x: number): number;
function over88(x: string): string;
function over88(x: number | string): number | string { return x; }
abstract class A88 { abstract f(x: number): number; }
class B88 extends A88 { override f(x: number): number { return x + 1; } }
enum E88 { A = 5 / 2, B, C = 2 ** 3, D }
const angle88 = <number>(88 + 1);
const check88 = {x: angle88} satisfies {x: number};
console.log(N88.add(1), over88(88), new B88().f(1), E88.B, E88.D, check88.x);
