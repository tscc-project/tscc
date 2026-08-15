namespace N94 {
  export const base: number = 94;
  export function add(x: number): number { return base + x; }
}
function over94(x: number): number;
function over94(x: string): string;
function over94(x: number | string): number | string { return x; }
abstract class A94 { abstract f(x: number): number; }
class B94 extends A94 { override f(x: number): number { return x + 1; } }
enum E94 { A = 5 / 2, B, C = 2 ** 3, D }
const angle94 = <number>(94 + 1);
const check94 = {x: angle94} satisfies {x: number};
console.log(N94.add(1), over94(94), new B94().f(1), E94.B, E94.D, check94.x);
