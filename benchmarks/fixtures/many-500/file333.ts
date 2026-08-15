interface Item333 { id: number; name: string; }
type Key333 = string | number;
export function make333(id: number, name: string): Item333 {
  const value: Item333 = {id, name};
  return value as Item333;
}
export const item333: Item333 = make333(333, "item-333");
