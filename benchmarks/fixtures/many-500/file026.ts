interface Item26 { id: number; name: string; }
type Key26 = string | number;
export function make26(id: number, name: string): Item26 {
  const value: Item26 = {id, name};
  return value as Item26;
}
export const item26: Item26 = make26(26, "item-26");
