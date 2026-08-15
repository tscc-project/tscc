namespace N90 {
  export const base: number = 90;
  export function add(x: number): number { return base + x; }
}
function over90(x: number): number;
function over90(x: string): string;
function over90(x: number | string): number | string { return x; }
abstract class A90 { abstract f(x: number): number; }
class B90 extends A90 { override f(x: number): number { return x + 1; } }
enum E90 { A = 5 / 2, B, C = 2 ** 3, D }
const angle90 = <number>(90 + 1);
const check90 = {x: angle90} satisfies {x: number};
console.log(N90.add(1), over90(90), new B90().f(1), E90.B, E90.D, check90.x);
