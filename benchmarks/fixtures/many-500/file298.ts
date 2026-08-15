interface Item298 { id: number; name: string; }
type Key298 = string | number;
export function make298(id: number, name: string): Item298 {
  const value: Item298 = {id, name};
  return value as Item298;
}
export const item298: Item298 = make298(298, "item-298");
