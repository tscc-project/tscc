interface Item257 { id: number; name: string; }
type Key257 = string | number;
export function make257(id: number, name: string): Item257 {
  const value: Item257 = {id, name};
  return value as Item257;
}
export const item257: Item257 = make257(257, "item-257");
