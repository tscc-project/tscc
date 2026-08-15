namespace N8 {
  export const base: number = 8;
  export function add(x: number): number { return base + x; }
}
function over8(x: number): number;
function over8(x: string): string;
function over8(x: number | string): number | string { return x; }
abstract class A8 { abstract f(x: number): number; }
class B8 extends A8 { override f(x: number): number { return x + 1; } }
enum E8 { A = 5 / 2, B, C = 2 ** 3, D }
const angle8 = <number>(8 + 1);
const check8 = {x: angle8} satisfies {x: number};
console.log(N8.add(1), over8(8), new B8().f(1), E8.B, E8.D, check8.x);
