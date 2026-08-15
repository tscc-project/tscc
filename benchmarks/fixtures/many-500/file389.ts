interface Item389 { id: number; name: string; }
type Key389 = string | number;
export function make389(id: number, name: string): Item389 {
  const value: Item389 = {id, name};
  return value as Item389;
}
export const item389: Item389 = make389(389, "item-389");
