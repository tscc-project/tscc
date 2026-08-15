interface Item286 { id: number; name: string; }
type Key286 = string | number;
export function make286(id: number, name: string): Item286 {
  const value: Item286 = {id, name};
  return value as Item286;
}
export const item286: Item286 = make286(286, "item-286");
