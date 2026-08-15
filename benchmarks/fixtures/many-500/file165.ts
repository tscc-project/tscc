interface Item165 { id: number; name: string; }
type Key165 = string | number;
export function make165(id: number, name: string): Item165 {
  const value: Item165 = {id, name};
  return value as Item165;
}
export const item165: Item165 = make165(165, "item-165");
