interface Item305 { id: number; name: string; }
type Key305 = string | number;
export function make305(id: number, name: string): Item305 {
  const value: Item305 = {id, name};
  return value as Item305;
}
export const item305: Item305 = make305(305, "item-305");
