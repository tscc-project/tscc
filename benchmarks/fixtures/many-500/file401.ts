interface Item401 { id: number; name: string; }
type Key401 = string | number;
export function make401(id: number, name: string): Item401 {
  const value: Item401 = {id, name};
  return value as Item401;
}
export const item401: Item401 = make401(401, "item-401");
