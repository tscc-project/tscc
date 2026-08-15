interface Item361 { id: number; name: string; }
type Key361 = string | number;
export function make361(id: number, name: string): Item361 {
  const value: Item361 = {id, name};
  return value as Item361;
}
export const item361: Item361 = make361(361, "item-361");
