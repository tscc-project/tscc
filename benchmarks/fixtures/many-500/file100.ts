interface Item100 { id: number; name: string; }
type Key100 = string | number;
export function make100(id: number, name: string): Item100 {
  const value: Item100 = {id, name};
  return value as Item100;
}
export const item100: Item100 = make100(100, "item-100");
