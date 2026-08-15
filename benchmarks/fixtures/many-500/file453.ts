interface Item453 { id: number; name: string; }
type Key453 = string | number;
export function make453(id: number, name: string): Item453 {
  const value: Item453 = {id, name};
  return value as Item453;
}
export const item453: Item453 = make453(453, "item-453");
