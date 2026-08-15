namespace N3 {
  export const base: number = 3;
  export function add(x: number): number { return base + x; }
}
function over3(x: number): number;
function over3(x: string): string;
function over3(x: number | string): number | string { return x; }
abstract class A3 { abstract f(x: number): number; }
class B3 extends A3 { override f(x: number): number { return x + 1; } }
enum E3 { A = 5 / 2, B, C = 2 ** 3, D }
const angle3 = <number>(3 + 1);
const check3 = {x: angle3} satisfies {x: number};
console.log(N3.add(1), over3(3), new B3().f(1), E3.B, E3.D, check3.x);
