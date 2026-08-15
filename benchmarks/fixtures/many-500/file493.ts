interface Item493 { id: number; name: string; }
type Key493 = string | number;
export function make493(id: number, name: string): Item493 {
  const value: Item493 = {id, name};
  return value as Item493;
}
export const item493: Item493 = make493(493, "item-493");
