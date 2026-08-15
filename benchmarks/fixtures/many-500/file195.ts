interface Item195 { id: number; name: string; }
type Key195 = string | number;
export function make195(id: number, name: string): Item195 {
  const value: Item195 = {id, name};
  return value as Item195;
}
export const item195: Item195 = make195(195, "item-195");
