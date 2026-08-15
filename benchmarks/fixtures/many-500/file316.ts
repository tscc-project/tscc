interface Item316 { id: number; name: string; }
type Key316 = string | number;
export function make316(id: number, name: string): Item316 {
  const value: Item316 = {id, name};
  return value as Item316;
}
export const item316: Item316 = make316(316, "item-316");
