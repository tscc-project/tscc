interface Item358 { id: number; name: string; }
type Key358 = string | number;
export function make358(id: number, name: string): Item358 {
  const value: Item358 = {id, name};
  return value as Item358;
}
export const item358: Item358 = make358(358, "item-358");
